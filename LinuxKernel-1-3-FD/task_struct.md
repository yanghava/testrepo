# Mapping of task_struct 

In a running process, the fields of task_struct are not accessible directly. They're mapped to /proc/\<pid\>/\<virtual files\> accordingly.

For example in an AES system, the TSAPI tsrv process has the process ID 8633

```
[root@aes102 ~]# ps -ef | grep tsrv
root        8629    4641  0 Jul14 ?        00:00:00 /usr/bin/sudo /opt/mvap/bin/tsrv
avaya       8633    8629  4 Jul14 ?        19:36:39 /opt/mvap/bin/tsrv
root     3652281 3652134  0 13:49 pts/0    00:00:00 grep --color=auto tsrv
``` 

looking into its status we have the following
```
[root@aes102 8633]# cat status
Name:   tsrv
Umask:  0027
State:  S (sleeping)
Tgid:   8633
Ngid:   0
Pid:    8633
PPid:   8629
TracerPid:      0
Uid:    0       504     504     504
Gid:    0       0       0       0
FDSize: 64
Groups: 0 504
NStgid: 8633
NSpid:  8633
NSpgid: 3996
NSsid:  3996
VmPeak:  1825136 kB
VmSize:  1825136 kB
VmLck:         0 kB
VmPin:         0 kB
VmHWM:     65048 kB
VmRSS:     65048 kB
RssAnon:           17328 kB
RssFile:            8256 kB
RssShmem:          39464 kB
VmData:   319428 kB
VmStk:       136 kB
VmExe:       544 kB
VmLib:     30696 kB
VmPTE:       756 kB
VmSwap:       36 kB
HugetlbPages:          0 kB
CoreDumping:    0
THP_enabled:    1
Threads:        39
SigQ:   3/14499
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: 0000000000000002
SigCgt: 0000000180001804
CapInh: 0000000000000000
CapPrm: 000001ffffffffff
CapEff: 0000000000000000
CapBnd: 000001ffffffffff
CapAmb: 0000000000000000
NoNewPrivs:     0
Seccomp:        0
Speculation_Store_Bypass:       thread vulnerable
Cpus_allowed:   3
Cpus_allowed_list:      0-1
Mems_allowed:   00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000001
Mems_allowed_list:      0
voluntary_ctxt_switches:        797997
nonvoluntary_ctxt_switches:     14
```

Note the ***Cpus_allowed*** and ***Cpus_allowed_list***. The mapping from the task_struct into the virtual status file can be found [here](https://github.com/torvalds/linux/blob/master/fs/proc/array.c#L405)

```c
static void task_cpus_allowed(struct seq_file *m, struct task_struct *task)
{
	seq_printf(m, "Cpus_allowed:\t%*pb\n",
		   cpumask_pr_args(&task->cpus_mask));
	seq_printf(m, "Cpus_allowed_list:\t%*pbl\n",
		   cpumask_pr_args(&task->cpus_mask));
}
```

To explain it: 
These lines show you which CPU cores the process (task) is allowed to run on (commonly known as its CPU Affinity). Here is a line-by-line breakdown of exactly what is happening under the hood.

1. Function Signature
	```c
	static void task_cpus_allowed(struct seq_file *m, struct task_struct *task)
	```
	- struct ***seq_file *m***: A seq_file is a kernel abstraction used to cleanly generate virtual text files in the /proc filesystem. Think of it as a write-buffer destined for the user's terminal.
	- struct ***task_struct *task***: This is the pointer to the target process's main descriptor

2. Printing the Hex Bitmask (Cpus_allowed)
	```c
	seq_printf(m, "Cpus_allowed:\t%*pb\n",
           cpumask_pr_args(&task->cpus_mask));
	```

	- ***&task->cpus_mask***: This accesses the cpus_mask field (which is of type struct cpumask / cpu_set_t) inside the process's task_struct. This mask uses individual bits to represent CPU cores (e.g., bit 0 for CPU0, bit 1 for CPU1, etc.).
	- ***%*pb***: This is a custom kernel-space print format specifier:
		- ***%p*** indicates a pointer format.
		- ***b*** tells the kernel to format this pointer as a bitmap.
		- ***\**** acts as a placeholder dynamically passing the size of the bitmap to the format string.

	- ***cpumask_pr_args(...)***: This is a helper macro defined in include/linux/cpumask.h. It expands the arguments into two distinct parameters required by %*pb:

		1. The size of the CPU mask (usually nr_cpu_ids).
		2. The actual bitmap pointer.

	- The ***Output***: A comma-separated, 32-bit grouped hexadecimal bitmask representation.
		- Example (for an 8-core system with access to all cores): Cpus_allowed: ff (where ff in binary is 11111111, representing cores 0 through 7).


3. Line 2: Printing the Human-Readable Range List (Cpus_allowed_list)
	```c
	seq_printf(m, "Cpus_allowed_list:\t%*pbl\n",
           cpumask_pr_args(&task->cpus_mask));
	```
	- ***%*pbl***: Similar to above, but the l suffix changes the output format from a hex bitmap to a range list of active CPU IDs.
	- ***The Output***: A human-readable list of CPU indices or ranges.
		- Example (if allowed to run on all 8 cores): Cpus_allowed_list: 0-7
		- Example (if pinned only to cores 0, 1, and 4): Cpus_allowed_list: 0-1,4

***Conclusion***
For the AES TSAPI tsrv service, the allowed CPU mask is 3 (00000011) so it can access only CPU 0 and CPU 1
```
Cpus_allowed:   3
Cpus_allowed_list:      0-1
```
