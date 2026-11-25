It is actually **PostgreSQL's standard default behavior on Linux** to use a Unix domain socket instead of TCP whenever the application and the database are on the same server!

Here is why PostgreSQL works this way, and **4 ways you can verify it** directly on your AES server.

---

### Why does PostgreSQL use a Unix Socket instead of TCP?
PostgreSQL listens on **both** TCP (for remote connections, or `127.0.0.1`) and a local Unix socket file (`/var/run/postgresql/.s.PGSQL.5430`) at the same time.

When a C/C++ program like `tsrv` (using PostgreSQL's `libpq` client library) connects locally without specifying an IP address, `libpq` automatically connects to **`/var/run/postgresql/.s.PGSQL.5430`** because:
1. **Higher Performance (~25–30% faster):** Data copies directly between `tsrv` and `postgres` in kernel memory, completely bypassing TCP/IP headers, ACK packets, checksums, and loopback routing.
2. **Better Security (`SO_PEERCRED`):** Over a Unix socket, the Linux kernel guarantees to PostgreSQL that the connecting process is running as Linux user `avaya` (PID `392720`), allowing secure local authentication without sending passwords over the network.

---

### 4 Ways to Verify This on Your AES Server

#### 1. Check the `postgres` worker process title (`ps -fp 392723`)
Whenever a client connects to PostgreSQL, PostgreSQL forks a dedicated backend process for that client (in your `lsof` output, that was **PID `392723`**) and writes the connection type right into the process title:

```bash
ps -fp 392723
```
You will see something like:
```text
UID     PID    PPID  C STIME TTY   TIME CMD
avaya 392723  ...   0 Oct28 ?   00:00:02 postgres: avaya mvap [local] idle
```
* In PostgreSQL, **`[local]`** specifically means **Unix domain socket connection**.
* If it were using a TCP socket, it would say `127.0.0.1(54321)` instead of `[local]`.

---

#### 2. Check both ends of the connection with `ss -xp`
Run `ss` filtering for PostgreSQL's socket path (`5430`):

```bash
ss -xp | grep 5430
```
You will see `postgres` (PID `392723`, FD `11`) on `/var/run/postgresql/.s.PGSQL.5430` paired directly with `tsrv` (PID `392720`, FD `4`) by their matching inode numbers (`122269172` $\leftrightarrow$ `122270755`).

```diff
[root@lnaes102 ~]# ps -fp 392723
UID          PID    PPID  C STIME TTY          TIME CMD
avaya     392723    3173  0 Oct28 ?        00:01:51 postgres: mvap_transport mvap [local] idle
[root@lnaes102 ~]# ss -xp | grep 5430
u_str ESTAB 0      0                        /var/run/postgresql/.s.PGSQL.5430 122259080            * 122259079 users:(("postgres",pid=388049,fd=11))                                                                                                                                                                                                                                                                                                  
u_str ESTAB 0      0                        /var/run/postgresql/.s.PGSQL.5430 122276534            * 122278507 users:(("postgres",pid=396361,fd=11))                                                                                                                                                                                                                                                                                                  
u_str ESTAB 0      0                        /var/run/postgresql/.s.PGSQL.5430 122287427            * 122291468 users:(("postgres",pid=396750,fd=11))                                                                                                                                                                                                                                                                                                  
-u_str ESTAB 0      0                        /var/run/postgresql/.s.PGSQL.5430 122269172            * 122270755 users:(("postgres",pid=392723,fd=11))                                                                                                                                                                                                                                                                                                  
u_str ESTAB 0      0                        /var/run/postgresql/.s.PGSQL.5430 122258061            * 122257066 users:(("postgres",pid=387794,fd=11))                                                                                                                                              
```
---

#### 3. Ask PostgreSQL itself (`pg_stat_activity`)
You can query PostgreSQL's internal connection table for backend PID `392723`:

```bash
psql -p 5430 -U avaya -d mvap -c "SELECT pid, usename, datname, client_addr, client_port, state FROM pg_stat_activity WHERE pid = 392723;"
```
*(Or run as `su - postgres -c "psql -p 5430 -c ..."`)*
* When **`client_addr` is blank (`NULL`)** and **`client_port` is `-1`**, PostgreSQL confirms the client is connected via a **Unix domain socket**.

```
[root@lnaes102 ~]# psql -p 5430 -U avaya -d mvap -c "SELECT pid, usename, datname, client_addr, client_port, state FROM pg_stat_activity WHERE pid = 392723;"
  pid   |    usename     | datname | client_addr | client_port | state
--------+----------------+---------+-------------+-------------+-------
 392723 | mvap_transport | mvap    |             |          -1 | idle
(1 row)

```
---

#### 4. Watch live SQL queries traveling over FD `4` (`strace`)
*(Optional — only on a lab/test AES server)*
Because FD `4` in `tsrv` (and FD `11` in `postgres` PID `392723`) is just a file descriptor, if `tsrv` runs a database query, you can actually see the SQL text being sent over FD `4`:

```bash
strace -p 392723 -e trace=recvfrom,sendto,read,write -s 200
```
Every time `tsrv` queries the database, you will see `postgres` (`392723`) reading the SQL query from **`fd 11`** (the Unix socket)!