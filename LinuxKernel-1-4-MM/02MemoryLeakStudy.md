# Simulating a Memory Leak & OOM

The course content can be found [here](https://avaya.atlassian.net/wiki/spaces/DLBBEWIKI/pages/2448850963/1-4+MM#Simulating-a-Memory-Leak-%26-OOM)

The source code is as below
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int loop_count = 0;

    printf("Process ID: %d\n", getpid());
    printf("\nPress Enter to start the test...\n");
    getchar();

    while (1) {
        char *ptr = (char *)malloc(100 * 1024);
                for (int i = 0; i < 100 * 1024; i++) {
            ptr[i] = 'A';
        }
        loop_count++;
                if (loop_count % 1024 == 0) {
                        printf("Leaked: %d00 MB total\n", loop_count/1024);
                }
        usleep(1000);
    }

    return 0;
}
```

compile and run
```
[root@DA22053700:LinuxKernel-1-4-MM]# gcc memoryLeak.c -o memoryLeak

[root@DA22053700:LinuxKernel-1-4-MM]# systemd-run --user --scope     -p MemoryMax=500M     -p MemorySwapMax=0     ./memoryLeak
Running scope as unit: run-r7894bb8ac81946fca0e16bf486759c11.scope
Process ID: 44571

Press Enter to start the test...

Leaked: 100 MB total
Leaked: 200 MB total
Leaked: 300 MB total
Leaked: 400 MB total
Killed
[root@DA22053700:LinuxKernel-1-4-MM]# systemd-run --user --scope     -p MemoryMax=500M     -p MemorySwapMax=500M     ./memoryLeak
Running scope as unit: run-r4b151134dae14cd0b51fcd038721ab0a.scope
Process ID: 44984

Press Enter to start the test...

Leaked: 100 MB total
Leaked: 200 MB total
Leaked: 300 MB total
Leaked: 400 MB total
Leaked: 500 MB total
Leaked: 600 MB total
Leaked: 700 MB total
Leaked: 800 MB total
Leaked: 900 MB total
Killed
```

Since I run the progarm within Ubuntu under WSL, I manually assigned the max VM the program can be allocated, (and the Swap memory), so the system will kill it once the memory allocation exceeds.

On the other hand, while the program runs, we have following observation on the process
```
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5a66f6000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      134608 133424  0.8
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5aa6d7000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      200220 198980  1.2
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5b6b29000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      401252 400128  2.4
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5befcc000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      536872 469916  2.8
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5c7023000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      668292 494728  3.0
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5cc134000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      751304 511644  3.1
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5d2d9f000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      862324 508836  3.1
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5d7456000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      934736 510040  3.1
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
62c59e64c000-62c5dc5fd000 rw-p 00000000 00:00 0                          [heap]

    PID    PPID COMMAND            VSZ   RSS %MEM
  44984   42071 memoryLeak      1018348 509940  3.1
[root@DA22053700:44984]# cat maps | grep heap;echo "" ; ps -p 44984 -o pid,ppid,comm,vsz,rss,%mem
cat: maps: No such process

    PID    PPID COMMAND            VSZ   RSS %MEM
```

Since each allocation is a small size, 100KB, the heap is extended continuously, until 1GB leakage happens and the program is killed. The virtual memory also continues increasing but the Resident memory won't increase after it reaches the 500MB maximum. 

