In the below output, we noticed a unix socket, with FD 13, is not communicating with any other process

```diff
[root@lnaes102 ~]# lsof -Ki -nP +E -a -c tsrv -d 4-6,13,17,18,20
COMMAND      PID  USER   FD   TYPE             DEVICE SIZE/OFF      NODE NAME
systemd        1  root   55u  unix 0xffff9b22876e6300      0t0     13776 /run/systemd/journal/dev-log type=DGRAM ->INO=26779 1481,crond,4u
systemd-j    709  root    3u  unix 0xffff9b22876e6300      0t0     13776 /run/systemd/journal/dev-log type=DGRAM ->INO=26779 1481,crond,4u
tsrv      392720 avaya    4u  unix 0xffff9b2298614380      0t0 122270755 type=STREAM ->INO=122269172 392723,postgres,11u
tsrv      392720 avaya    5u  unix 0xffff9b2298616780      0t0 122270757 type=STREAM ->INO=122270758 392720,tsrv,6u
tsrv      392720 avaya    6u  unix 0xffff9b2298617980      0t0 122270758 type=STREAM ->INO=122270757 392720,tsrv,5u
-tsrv      392720 avaya   13u  unix 0xffff9b2298614800      0t0 122270766 type=STREAM
tsrv      392720 avaya   17u  unix 0xffff9b2298617500      0t0 122270775 type=STREAM ->INO=122270776 392720,tsrv,18u
tsrv      392720 avaya   18u  unix 0xffff9b2298612880      0t0 122270776 type=STREAM ->INO=122270775 392720,tsrv,17u
tsrv      392720 avaya   20u  unix 0xffff9b2298612d00      0t0 122270778 type=DGRAM ->INO=13776 709,systemd-j,3u 1,systemd,55u
postgres  392723 avaya   11u  unix 0xffff9b2298615a00      0t0 122269172 /var/run/postgresql/.s.PGSQL.5430 type=STREAM ->INO=122270755 392720,tsrv,4u

```



First, notice how much `lsof +E` just revealed about your `tsrv` process:
* **`4u` (`122270755`):** Connected to **`postgres`** (PID `392723`, FD `11u`) via `/var/run/postgresql/.s.PGSQL.5430` (database connection).
* **`5u` $\leftrightarrow$ `6u` (`122270757` $\leftrightarrow$ `122270758`):** Connected to **each other** inside `tsrv` (`socketpair` used for internal thread-to-thread signaling).
* **`17u` $\leftrightarrow$ `18u` (`122270775` $\leftrightarrow$ `122270776`):** Another internal `socketpair` inside `tsrv`.
* **`20u` (`122270778`):** Connected to **`systemd-journald`** (`/run/systemd/journal/dev-log`) for writing system logs.

---

### Now, what is happening with FD `13u` (`122270766`)?

Look closely at what `ss -xap` returned for `122270766`:

```text
State  Recv-Q  Send-Q  Local Address:Port   Peer Address:Port  Process
u_str  ESTAB   0       * 122270766          * 0                users:(("tsrv",pid=392720,fd=13))
```

Notice two seemingly contradictory things:
1. The state is **`ESTAB`** (Established — meaning this socket **was** connected).
2. The Peer Port (Peer Inode) is **`0`** (`* 0`), and `lsof +E` shows no `->INO=...`.

#### Why does this happen in the Linux kernel?
In the Linux kernel (`net/unix/af_unix.c`), a Unix stream socket (`u_str`) is in state **`ESTAB` with Peer Inode `0`** when **the socket was connected, but the other end has already `close()`d its file descriptor** (a "half-closed" or orphaned Unix socket):

1. Originally, FD `13u` (`122270766`) was connected to a peer socket (for example, inode `122270767`, created via `socketpair()` or a local client connection).
2. At that time, both sockets had state `ESTAB`.
3. Later, **the other end called `close()`** on its file descriptor (either another process exited/closed its FD, or `tsrv` itself closed the other half of a `socketpair`), **while `tsrv` kept FD `13` open**.
4. When the peer's FD was closed, the Linux Virtual File System freed the peer's inode, so the kernel function `sock_i_ino(peer)` now returns **`0`**.
5. Because `tsrv` has not called `close(13)` yet, FD `13u` remains in `tsrv`'s file descriptor table in the `ESTAB` state with peer inode `0`.

---

### How to inspect FD `13u` further

You can verify the exact kernel state of `122270766` using these two commands:

#### 1. Check `/proc/net/unix` directly
```bash
head -1 /proc/net/unix; grep 122270766 /proc/net/unix
```
* Look at the **`St` (State)** column:
  * `01` = Unconnected (`SS_UNCONNECTED`)
  * `02` = Connecting (`SS_CONNECTING`)
  * `03` = Connected (`SS_CONNECTED` — even if the peer later closed its FD)
* Look at **`RefCount`**: It will likely be `00000002` (smaller than an active connected socket like `5u` which usually has `00000003` because the peer also holds a reference).

#### 2. Check detailed socket flags and shutdown state in `/proc/<PID>/fdinfo/13`
```bash
cat /proc/392720/fdinfo/13
```
And with `ss` extended memory/state info:
```bash
ss -xapei | grep -A 1 122270766
```
*(In short: FD `13u` is simply a socket whose peer already closed its side of the connection, and `tsrv` either only uses it one-way or never cleaned up FD `13` after startup).*

In your specific case with `tsrv`, **leaving FD `13u` open has virtually zero impact**—it is completely harmless. 

Here is why, along with when leaving half-closed sockets open *does* become a problem in Linux:

---

### 1. Why `13u` is harmless right now
* **Tiny memory footprint:** A single idle Unix socket only consumes about **1–2 KB** of kernel memory (for the `struct sock` metadata) and **1 file descriptor number** (`13`).
* **No buffered data stuck in RAM:** In your `ss -xap` output, both `Recv-Q` and `Send-Q` are **`0`**:
  ```text
  u_str ESTAB  0  0  * 122270766  * 0
  ```
  So no unread messages are sitting in kernel memory.
* **It is not leaking over time:** From your very first `ls -l /proc/392720/fd` output, `tsrv` has been running since **Oct 28** and only has **26 file descriptors open in total** (`0` through `25`). That proves FD `13u` was created once (likely during startup/initialization) and is not multiplying.

---

### 2. When DOES leaving half-closed sockets open cause real problems?
In networking and systems engineering, failing to `close()` sockets after the peer disconnects only becomes dangerous if it happens **repeatedly** (known as a **File Descriptor Leak**) or is **mishandled in an event loop**:

#### Problem A: Exhausting the File Descriptor Limit (`Too many open files`)
* Every Linux process has a hard cap on how many file descriptors it can have open at once (often `1024` or `4096` by default).
* You can check `tsrv`'s limit with:
  ```bash
  grep "Max open files" /proc/392720/limits
  ```
* If a server forgets to call `close()` every time a client disconnects, thousands of half-closed sockets (or `CLOSE_WAIT` TCP sockets) will pile up until the process hits its limit. Once full, `accept()` fails with `EMFILE (Too many open files)` and the server stops accepting new connections.

#### Problem B: Wasted Kernel Memory (if `Recv-Q > 0`)
* If the peer sent data before closing its socket, and the receiving process neither reads the data nor closes its FD, that data remains locked in non-swappable kernel memory (`Recv-Q`) forever until the FD is closed or the process restarts.

#### Problem C: 100% CPU Spin (if left in `select()` / `poll()` / `epoll`)
* When a peer closes its end of a stream socket, the kernel permanently marks the socket as **"readable" (EOF)** so the program wakes up, reads `0` bytes, and knows to call `close()`.
* If a buggy program leaves that FD inside a `select()` or `poll()` loop without closing or removing it after receiving `0` bytes (EOF), `poll()` will return immediately on every loop iteration, pinning one thread at **100% CPU**. (Your `tsrv` is at `4%` CPU total across 30+ threads, so it is not spinning on FD `13u`).

From the **`ps -ef | grep tsrv`** output at the very top of your first message!

Here is that line aligned with the standard `ps -ef` column headers:

```text
UID      PID     PPID   C  STIME  TTY     TIME      CMD
avaya  392720  392698   4  Oct28  ?     05:37:38  /opt/mvap/bin/tsrv
```

There are two clues right in that line:

1. **`C` column = `4`:**
   In `ps -ef`, the 4th column (`C`) is the **CPU utilization percentage** over the lifetime of the process (`4%`).
2. **`TIME` column = `05:37:38` (with `STIME` = `Oct28`):**
   * The process started on **Oct 28** and your command was run on **Nov 2 at 21:31** (~5.25 days, or ~126 hours of wall-clock time).
   * In ~126 hours of running, `tsrv` consumed **5 hours and 37 minutes** (`5.62` hours) of total CPU time:
     $$\frac{5.62\text{ CPU hours}}{126\text{ wall-clock hours}} \approx 4.4\%$$

If a thread were stuck in an infinite loop on a half-closed socket, it would burn **1 full CPU core continuously** (`100%` in column `C`), and after 5.25 days its `TIME` column would be over **126:00:00**!