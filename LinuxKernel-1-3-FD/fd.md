# FD
refer to the course [here](https://avaya.atlassian.net/wiki/spaces/DLBBEWIKI/pages/2429485226/1-3+FD#FD)

# AES TSAPI service logging

TSAPI service logs are stored under /var/log/avaya/aes/TSAPI

```
[root@aes102 TSAPI]# ls -l
total 732
-rw-r--r-- 1 avaya susers 329358 Aug  2 09:21 audit_trace.out
-rw-r--r-- 1 avaya susers 104487 Aug  2 09:21 csta_trace.out
-rw-r--r-- 1 avaya susers   5100 Jul 14 02:38 csta_trace.out-2026-07-14-1783996801.gz
-rw-r--r-- 1 avaya susers   5202 Aug  2 08:54 csta_trace.out-2026-08-02-1785660961.gz
-rw-r--r-- 1 avaya susers  73924 Aug  2 09:21 g3trace.out
-rw-r--r-- 1 avaya susers    253 Jul 14 02:37 g3trace.out-2026-07-14-1783996681.gz
-rw-r--r-- 1 avaya susers   2946 Jul 14 02:38 g3trace.out-2026-07-15-1784073601.gz
-rw-r--r-- 1 avaya susers    256 Aug  2 07:57 g3trace.out-2026-08-02-1785657481.gz
drwxr-x--- 3 root  root       18 Aug 17  2023 mgmt
```

The log files are written by rsyslog

```
[root@aes102 TSAPI]# lsof csta_trace.out
COMMAND   PID USER   FD   TYPE DEVICE SIZE/OFF     NODE NAME
rsyslogd 1431 root   32w   REG  253,8   104487 16832799 csta_trace.out
```

This can be verified using fd virtual files under rsyslog process
```
[root@aes102 fd]# ls -l | grep csta_trace
l-wx------ 1 root root 64 Aug  2 08:52 32 -> /var/log/avaya/aes/TSAPI/csta_trace.out
```

look more for what rsyslog writes for AES services
```
[root@aes102 fd]# ls -l | grep aes
l-wx------ 1 root root 64 Aug  2 08:52 11 -> /var/log/avaya/aes/common/trace.out
l-wx------ 1 root root 64 Aug  2 08:52 12 -> /var/log/avaya/aes/cmd.log
l-wx------ 1 root root 64 Aug  2 08:52 14 -> /var/log/avaya/aes/mvap.log
l-wx------ 1 root root 64 Aug  2 08:52 16 -> /var/log/avaya/aes/alarm.log
l-wx------ 1 root root 64 Aug  2 08:52 18 -> /var/log/avaya/aes/trans_serv/trace.out
l-wx------ 1 root root 64 Aug  2 08:52 19 -> /var/log/avaya/aes/mvap.log
l-wx------ 1 root root 64 Aug  2 08:52 20 -> /var/log/avaya/aes/mvap.log
l-wx------ 1 root root 64 Aug  2 08:52 21 -> /var/log/avaya/aes/common/trace.out
l-wx------ 1 root root 64 Aug  2 08:52 22 -> /var/log/avaya/aes/common/trace.out
l-wx------ 1 root root 64 Aug  2 08:52 23 -> /var/log/avaya/aes/TSAPI/audit_trace.out
l-wx------ 1 root root 64 Aug  2 08:52 25 -> /var/log/avaya/aes/TSAPI/g3trace.out
l-wx------ 1 root root 64 Aug  2 08:52 27 -> /var/log/avaya/aes/lcm-trace.log
l-wx------ 1 root root 64 Aug  2 08:52 28 -> /var/log/avaya/aes/dmcc-trace.log
l-wx------ 1 root root 64 Aug  2 08:52 29 -> /var/log/avaya/aes/snmp-subagent.log
l-wx------ 1 root root 64 Aug  2 08:52 30 -> /var/log/avaya/aes/dmcc-api.log
l-wx------ 1 root root 64 Aug  2 08:52 31 -> /var/log/avaya/aes/dmcc-jtapi-error.log
l-wx------ 1 root root 64 Aug  2 08:52 32 -> /var/log/avaya/aes/TSAPI/csta_trace.out
l-wx------ 1 root root 64 Aug  2 08:52 33 -> /var/log/avaya/aes/sec.log
l-wx------ 1 root root 64 Aug  2 08:52 34 -> /var/log/avaya/aes/dmcc-nist.log
l-wx------ 1 root root 64 Aug  2 08:52 35 -> /var/log/avaya/aes/telrestsvc.log
```





