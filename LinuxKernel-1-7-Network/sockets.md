In Linux, when a program creates a socket using the system call `socket(domain, type, protocol)`, sockets are categorized along **two main dimensions**:

1. **Domain (Address Family):** *Where* the endpoints are (same machine vs. across a network).
2. **Socket Type (Communication Semantics):** *How* data is delivered (reliable stream vs. individual packets).

---

### Dimension 1: By Domain / Address Family (*Where* they communicate)
This is what `lsof` displays in the **`TYPE`** column:

| Socket Domain | C Constant | `lsof` `TYPE` | How It Works & Common Use Cases |
| :--- | :--- | :--- | :--- |
| **Unix Domain Socket** | `AF_UNIX` (or `AF_LOCAL`) | `unix` | **Same-machine only (IPC).** Data goes directly through kernel memory without touching the TCP/IP network stack, IP addresses, or network cards. Extremely fast and low latency.<br>*Examples:* Docker daemon (`/var/run/docker.sock`), MySQL local socket, `rsyslog` (`/dev/log`), or internal threads/processes in your `tsrv` (`4u`, `5u`, `6u`). |
| **IPv4 Socket** | `AF_INET` | `IPv4` | **Network communication over IPv4** (`32-bit` IP address + `16-bit` port). Can communicate locally via loopback (`127.0.0.1`) or over a physical network.<br>*Example:* FD `7u` (`UDP 127.0.0.1:62447->127.0.0.1:5510`). |
| **IPv6 Socket** | `AF_INET6` | `IPv6` | **Network communication over IPv6** (`128-bit` IP address + `16-bit` port). On Linux, an IPv6 wildcard socket (`*`) is **dual-stack** by default, handling both IPv4 and IPv6 traffic.<br>*Example:* FDs `14u`, `15u`, `24u` in your `tsrv` output. |
| **Netlink Socket** | `AF_NETLINK` | `netlink` | **Kernel $\leftrightarrow$ User-space communication.** Used by system tools to query or configure the Linux kernel (networking tables, firewall rules, hardware events).<br>*Examples:* `ip addr`, `ss`, `NetworkManager`, `udev`. |
| **Packet Socket** | `AF_PACKET` | `pack` | **Layer 2 (Ethernet frame) raw access.** Captures or injects raw frames directly at the network card driver level.<br>*Examples:* `tcpdump`, `Wireshark`, `dhclient`. |

---

### Dimension 2: By Communication Type (*How* data moves)
This is what `lsof` shows in the **`NODE`** column (for IP sockets) or **`NAME`** column (for Unix sockets):

#### 1. Stream Sockets (`SOCK_STREAM`)
* **Like a phone call:** Connection-oriented, reliable, two-way continuous byte stream.
* Guarantees data arrives **in order**, **without duplicates**, and **retransmits lost data**.
* Does **not** preserve message boundaries (if you `send()` 100 bytes twice, the receiver might `recv()` all 200 bytes at once).
* **In IPv4/IPv6:** Uses **TCP** (e.g., `tsrv` FDs `14u`–`16u`, `21u`–`25u`).
* **In Unix Domain:** Shown as **`type=STREAM`** (e.g., `tsrv` FDs `4u`–`6u`, `13u`, `17u`, `18u`).

#### 2. Datagram Sockets (`SOCK_DGRAM`)
* **Like sending letters/postcards:** Connectionless, message-oriented.
* Preserves **message boundaries** (if you send a 100-byte packet, the receiver gets that exact 100-byte packet).
* Over a network, there is no handshake, no retransmission, and packets can arrive out of order or be dropped—making it very fast with low overhead.
* **In IPv4/IPv6:** Uses **UDP** (e.g., `tsrv` FD `7u`, DNS, RTP voice/media streams, SNMP).
* **In Unix Domain:** Shown as **`type=DGRAM`** (e.g., `tsrv` FD `20u`—reliable because it's in local kernel memory, commonly used for sending log messages).

#### 3. Raw Sockets (`SOCK_RAW`)
* **Direct protocol access:** Bypasses the transport layer (TCP/UDP) so the application can construct its own protocol headers (like ICMP or custom IP packets).
* Requires `root` privileges (or `CAP_NET_RAW`).
* *Examples:* `ping` (uses ICMP), `traceroute`, `nmap`, routing daemons (OSPF).

#### 4. Sequenced Packet Sockets (`SOCK_SEQPACKET`)
* **Hybrid of Stream + Datagram:** Connection-oriented and reliable like TCP (`SOCK_STREAM`), but preserves message boundaries like UDP (`SOCK_DGRAM`).
* Used in **SCTP** (common in telecom/signaling) and local `AF_UNIX` IPC.

---

### Bonus: The Two Roles of a TCP (`SOCK_STREAM`) Socket
When looking at your `tsrv` output on port `1050`, you saw two different states for TCP sockets:

```text
tsrv 392720 avaya 23u IPv6 122269174 0t0 TCP *:1050 (LISTEN)
tsrv 392720 avaya 24u IPv6 122287528 0t0 TCP 127.0.0.1:1050->127.0.0.1:60380 (ESTABLISHED)
tsrv 392720 avaya 25u IPv6 122287540 0t0 TCP 127.0.0.1:1050->127.0.0.1:60384 (ESTABLISHED)
```

1. **Listening Socket (`23u` — `LISTEN`):**
   * Acts like a **receptionist** at the front desk.
   * It only listens on `*:1050` for new clients performing a TCP 3-way handshake (`SYN`, `SYN-ACK`, `ACK`). It never sends or receives actual application data.
2. **Connected / Data Socket (`24u`, `25u` — `ESTABLISHED`):**
   * As soon as a client finishes the handshake with `23u`, the `accept()` call creates a **brand-new socket FD** (`24u` for the first client, `25u` for the second client).
   * Each connected socket is uniquely identified by a **4-tuple**: `(Local IP, Local Port, Remote IP, Remote Port)` and handles all data transfer with that specific client.
   

Every connected Unix socket has **two endpoints** (two kernel inode numbers paired together: your socket's inode and the peer socket's inode).

You can see which two processes are talking to each other using either **`lsof +E`** or **`ss -xap`**:

---

### Method 1: Use `lsof +E` (Easiest to read)
Add the **`+E`** flag to your `lsof` command. It appends the **peer inode (`->INO=`)** and the **peer's `PID,COMMAND,FD`** directly to the end of each line:

```bash
lsof -Ki -nP +E -a -c tsrv -d 4-6,13,17,18,20
```

#### How to read the output:
You will see output that looks like one of these two cases at the end of each line:

1. **Talking to another process:**
   ```text
   tsrv      392720 avaya    4u  unix 0xffff9b2298614380      0t0 122270755 type=STREAM ->INO=122269172 392723,postgres,11u
   ```
   * **`122270755`** is `tsrv`'s socket (PID `392720`, FD `4u`).
   * **`->INO=122270111 391000,asai,9u`** means the other end is inode `122270111`, owned by process **`asai` (PID `391000`)** on its **FD `9u`**.

2. **Talking to itself (Internal Thread-to-Thread signaling via `socketpair`):**
   ```text
   tsrv 392720 avaya 5u unix 0xffff... 0t0 122270757 type=STREAM ->INO=122270758 392720,tsrv,6u
   tsrv 392720 avaya 6u unix 0xffff... 0t0 122270758 type=STREAM ->INO=122270757 392720,tsrv,5u
   ```
   * Notice in your earlier output that FDs `5u` (`122270757`) and `6u` (`122270758`) have consecutive inode numbers!
   * Multi-threaded C/C++ servers very often call `socketpair()` to create two connected FDs (`5u` and `6u`) inside the **same process** so one thread can wake up another thread by writing a byte to `5u` and reading it on `6u`.

---

### Method 2: Use `ss -xap` (Fastest)
In `ss` (Socket Statistics), for Unix sockets (`-x`), the **"Port"** column is the **inode number**, and the **"Peer Port"** column is the **other end's inode number**.

If you `grep` for a socket's inode (for example, `122270755` from your `4u`), `ss` will return **both** lines of the connection at once:

```bash
ss -xap | grep 122270755
```

#### Example output:
```text
u_str ESTAB 0 0 * 122270755 * 122270756 users:(("tsrv",pid=392720,fd=4))
u_str ESTAB 0 0 * 122270756 * 122270755 users:(("other_proc",pid=12345,fd=8))
```
* **Line 1:** Inode `122270755` (`tsrv`, PID `392720`, FD `4`) is connected to peer inode `122270756`.
* **Line 2:** Inode `122270756` (`other_proc`, PID `12345`, FD `8`) is connected to peer inode `122270755`.

#### output from AES
```diff
[root@lnaes102 ~]# lsof -Ki -nP +E -a -c tsrv -d 4-6,13,17,18,20
COMMAND      PID  USER   FD   TYPE             DEVICE SIZE/OFF      NODE NAME
systemd        1  root   55u  unix 0xffff9b22876e6300      0t0     13776 /run/systemd/journal/dev-log type=DGRAM ->INO=26779 1481,crond,4u
systemd-j    709  root    3u  unix 0xffff9b22876e6300      0t0     13776 /run/systemd/journal/dev-log type=DGRAM ->INO=26779 1481,crond,4u
+tsrv      392720 avaya    4u  unix 0xffff9b2298614380      0t0 122270755 type=STREAM ->INO=122269172 392723,postgres,11u
tsrv      392720 avaya    5u  unix 0xffff9b2298616780      0t0 122270757 type=STREAM ->INO=122270758 392720,tsrv,6u
tsrv      392720 avaya    6u  unix 0xffff9b2298617980      0t0 122270758 type=STREAM ->INO=122270757 392720,tsrv,5u
-tsrv      392720 avaya   13u  unix 0xffff9b2298614800      0t0 122270766 type=STREAM
tsrv      392720 avaya   17u  unix 0xffff9b2298617500      0t0 122270775 type=STREAM ->INO=122270776 392720,tsrv,18u
tsrv      392720 avaya   18u  unix 0xffff9b2298612880      0t0 122270776 type=STREAM ->INO=122270775 392720,tsrv,17u
tsrv      392720 avaya   20u  unix 0xffff9b2298612d00      0t0 122270778 type=DGRAM ->INO=13776 709,systemd-j,3u 1,systemd,55u
+postgres  392723 avaya   11u  unix 0xffff9b2298615a00      0t0 122269172 /var/run/postgresql/.s.PGSQL.5430 type=STREAM ->INO=122270755 392720,tsrv,4u
```
```
[root@lnaes102 ~]# ss -xap | grep 122270755
u_str ESTAB  0      0                                                        * 122270755            * 122269172 users:(("tsrv",pid=392720,fd=4))                                                                                                                                                                                                                                                                                                      
u_str ESTAB  0      0                        /var/run/postgresql/.s.PGSQL.5430 122269172            * 122270755 users:(("postgres",pid=392723,fd=11))             
```
**Note：**
* **FD 13**: the tsrv has a [half closed socket](zombinSocket.md)
* **FD 4**: the tsrv communicates with postgres with [unix socket](postgresSocket.md) instead of tcp socket.
