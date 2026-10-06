# Process Schedule in details

explain below 
```
[root@lnaes102 44715]# ls
attr                environ    mountinfo      personality   statm
autogroup           exe        mounts         projid_map    status
auxv                fd         mountstats     root          syscall
cgroup              fdinfo     net            sched         task
clear_refs          gid_map    ns             schedstat     timens_offsets
cmdline             io         numa_maps      sessionid     timers
comm                limits     oom_adj        setgroups     timerslack_ns
coredump_filter     loginuid   oom_score      smaps         uid_map
cpu_resctrl_groups  map_files  oom_score_adj  smaps_rollup  wchan
cpuset              maps       pagemap        stack
cwd                 mem        patch_state    stat

[root@lnaes102 44715]# cat cmdline
/opt/mvap/bin/tsrv[root@lnaes102 44715]#

[root@lnaes102 44715]# cat schedstat
1944250059 1227957696 83053

[root@lnaes102 44715]# cat sched
tsrv (44715, #threads: 31)
-------------------------------------------------------------------
se.exec_start                                :     166306038.342891
se.vruntime                                  :     251997372.755348
se.sum_exec_runtime                          :          1944.293546
se.nr_migrations                             :                 1684
nr_switches                                  :                83055
nr_voluntary_switches                        :                83050
nr_involuntary_switches                      :                    5
se.load.weight                               :              1048576
se.avg.load_sum                              :                   21
se.avg.runnable_sum                          :                21504
se.avg.util_sum                              :                21504
se.avg.load_avg                              :                    0
se.avg.runnable_avg                          :                    0
se.avg.util_avg                              :                    0
se.avg.last_update_time                      :      166306038342656
se.avg.util_est.ewma                         :                    9
se.avg.util_est.enqueued                     :                    0
policy                                       :                    0
prio                                         :                  120
clock-delta                                  :                   36
mm->numa_scan_seq                            :                    0
numa_pages_migrated                          :                    0
numa_preferred_nid                           :                   -1
total_numa_faults                            :                    0
current_node=0, numa_group_id=0
numa_faults node=0 task_private=0 task_shared=0 group_private=0 group_shared=0
```
The directory is `/proc/44715`, where `44715` is the process ID. It contains kernel-maintained information about the process, its memory, files, scheduling, namespaces, threads, and more.

## 1. `cmdline`

```text
/opt/mvap/bin/tsrv
```

This is the process command line. The file contains NUL-separated arguments rather than newline-separated text, so `cat` does not add a visible line break.

Use this to display arguments clearly:

```bash
tr '\0' ' ' < /proc/44715/cmdline
echo
```

The process executable is:

```text
/opt/mvap/bin/tsrv
```

## 2. `schedstat`

```text
1944250059 1227957696 83053
```

For this kernel interface, the three values are generally:

```text
<CPU execution time> <run-queue wait time> <timeslice/context-switch count>
```

The time values are in nanoseconds.

Converted:

```text
CPU execution time:  1,944,250,059 ns ≈ 1.944 seconds
Run-queue delay:     1,227,957,696 ns ≈ 1.228 seconds
Count:               83,053
```

So, since the process started, this task has accumulated approximately:

- 🟢 **1.944 seconds actually executing on a CPU**
- 🟡 **1.228 seconds waiting to be scheduled**
- **83,053 scheduler events**, normally interpreted as scheduler switch events

The exact interpretation of the third field can depend on the kernel version and configuration. The more detailed `/proc/44715/sched` output reports related switch counters.

## 3. Process and thread count

```text
tsrv (44715, #threads: 31)
```

This indicates:

- Process/thread-group name: `tsrv`
- Thread-group ID or main PID: `44715`
- Number of threads: **31**

The files directly under `/proc/44715/` describe the main thread or process-level view. Individual thread information is available under:

```bash
/proc/44715/task/
```

List the thread IDs:

```bash
#the process ID changed due to a restrat while investigation was performed.

[root@lnaes102 task]# pwd;ls
/proc/3208564/task
3208564  3208582  3208630  3208636  3208640  3208657  3208661  3208665
3208569  3208610  3208631  3208637  3208648  3208658  3208662  3213964
3208577  3208611  3208633  3208638  3208655  3208659  3208663  3213968
3208579  3208617  3208634  3208639  3208656  3208660  3208664

```

Inspect a particular thread:

```bash
cat /proc/44715/task/<TID>/sched
cat /proc/44715/task/<TID>/schedstat
```

For example
```
[root@lnaes102 44715]# cat sched
tsrv (44715, #threads: 31)
-------------------------------------------------------------------
se.exec_start                                :     166306038.342891
se.vruntime                                  :     251997372.755348
se.sum_exec_runtime                          :          1944.293546
se.nr_migrations                             :                 1684
nr_switches                                  :                83055
nr_voluntary_switches                        :                83050
nr_involuntary_switches                      :                    5
se.load.weight                               :              1048576
se.avg.load_sum                              :                   21
se.avg.runnable_sum                          :                21504
se.avg.util_sum                              :                21504
se.avg.load_avg                              :                    0
se.avg.runnable_avg                          :                    0
se.avg.util_avg                              :                    0
se.avg.last_update_time                      :      166306038342656
se.avg.util_est.ewma                         :                    9
se.avg.util_est.enqueued                     :                    0
policy                                       :                    0
prio                                         :                  120
clock-delta                                  :                   36
mm->numa_scan_seq                            :                    0
numa_pages_migrated                          :                    0
numa_preferred_nid                           :                   -1
total_numa_faults                            :                    0
current_node=0, numa_group_id=0
numa_faults node=0 task_private=0 task_shared=0 group_private=0 group_shared=0
```

## 4. CPU execution and scheduling fields

### `se.exec_start`

```text
se.exec_start : 166306038.342891
```

This is an internal scheduler timestamp associated with when the task began its most recent execution period. It is based on the kernel’s scheduling clock and is not a wall-clock timestamp.

### `se.vruntime`

```text
se.vruntime : 251997372.755348
```

This is the task’s **virtual runtime** used by the Completely Fair Scheduler, or CFS.

CFS uses virtual runtime to decide which runnable task should execute next. A task with relatively lower virtual runtime is generally favored over other runnable tasks in the same scheduling group.

It is not directly equivalent to elapsed time, CPU time, or a timestamp.

### `se.sum_exec_runtime`

```text
se.sum_exec_runtime : 1944.293546
```

This is the total CPU execution time accumulated by the task, in milliseconds:

```text
1944.293546 ms ≈ 1.944 seconds
```

This matches the first `schedstat` value:

```text
1,944,250,059 ns ≈ 1,944.25 ms
```

Small differences are normal because the values may be read at slightly different times.

### `se.nr_migrations`

```text
se.nr_migrations : 1684
```

The task has been migrated between CPUs **1,684 times**.

This does not mean it used 1,684 CPUs; it means the scheduler moved it between CPUs that many times.

### `nr_switches`

```text
nr_switches : 83055
```

Total number of scheduler switches recorded for this task.

The value differs slightly from the `schedstat` value:

```text
schedstat: 83053
sched:     83055
```

This can occur because the values are read at different moments or because the interfaces use slightly different accounting paths.

### Voluntary and involuntary switches

```text
nr_voluntary_switches   : 83050
nr_involuntary_switches : 5
```

Interpretation:

- **Voluntary switches: 83,050**  
  The task gave up the CPU, usually by sleeping, blocking on I/O, waiting for a lock, waiting for a condition, or calling a scheduling-related operation.

- **Involuntary switches: 5**  
  The scheduler took the CPU away from the task, for example because its time slice expired or a higher-priority task became runnable.

This process is therefore spending most of its scheduling transitions voluntarily waiting rather than being forcibly preempted.

## 5. Scheduling policy and priority

```text
policy : 0
prio   : 120
```

### `policy: 0`

Policy value `0` normally means:

```text
SCHED_NORMAL
```

This is the standard time-sharing policy used by ordinary processes. It is also commonly referred to as `SCHED_OTHER`.

Check it using:

```bash
chrt -p 44715
```

Expected result:

```text
pid 44715's current scheduling policy: SCHED_OTHER
pid 44715's current scheduling priority: 0
```

### `prio: 120`

For a normal CFS task, priority `120` corresponds to a nice value of `0`.

Check the nice value with:

```bash
ps -p 44715 -o pid,ni,pri,cls,stat,cmd
```

The relationship for normal tasks is approximately:

```text
kernel priority = 120 + nice value
```

For example:

```text
nice  0 → prio 120
nice  5 → prio 125
nice -5 → prio 115
```

Real-time scheduling policies use a different priority interpretation.

## 6. Load weight

```text
se.load.weight : 1048576
```

This is the scheduler weight assigned to the task. For a normal task with nice value `0`, the default CFS weight is commonly:

```text
1,048,576
```

A higher nice value reduces the weight and gives the task less CPU share relative to competing tasks. A lower nice value increases the weight.

This is not a percentage of CPU usage.

## 7. Scheduler accounting averages

```text
se.avg.load_sum      : 21
se.avg.runnable_sum  : 21504
se.avg.util_sum      : 21504
se.avg.load_avg      : 0
se.avg.runnable_avg  : 0
se.avg.util_avg      : 0
```

These are internal **PELT scheduler statistics**:

- `load_*` — task load contribution
- `runnable_*` — how much the task has been runnable
- `util_*` — estimated CPU utilization
- `*_sum` — accumulated internal values
- `*_avg` — scaled/decayed averages

The zero averages do not mean the process has never used CPU. They indicate that, at the instant of inspection, the task may not have been currently enqueued or runnable, or that the decayed accounting values had become zero.

These fields are kernel implementation details and can vary by Linux version.

### Utilization estimate

```text
se.avg.util_est.ewma     : 9
se.avg.util_est.enqueued : 0
```

`util_est.ewma` is an exponentially weighted estimate of the task’s recent CPU utilization. A value of `9` is low on the kernel’s internal utilization scale.

`enqueued: 0` indicates that the task was not currently accounted as enqueued on a run queue when the file was read.

## 8. `clock-delta`

```text
clock-delta : 36
```

This is an internal scheduler diagnostic value representing a small measured clock-difference or clock-read overhead. It is not the task’s CPU runtime and is generally not useful for application-level analysis.

## 9. NUMA information

```text
mm->numa_scan_seq       : 0
numa_pages_migrated     : 0
numa_preferred_nid      : -1
total_numa_faults       : 0
current_node=0
numa_group_id=0
```

These fields relate to NUMA memory scheduling.

### `mm->numa_scan_seq`

```text
0
```

NUMA balancing scan sequence for the process’s memory descriptor. A zero value indicates no NUMA balancing scan sequence has been recorded for this process, or that NUMA balancing is not actively applicable.

### `numa_pages_migrated`

```text
0
```

No pages have been migrated by automatic NUMA balancing for this task.

### `numa_preferred_nid`

```text
-1
```

No preferred NUMA node has been assigned.

### `total_numa_faults`

```text
0
```

No NUMA page-fault accounting events are shown for this task.

### `current_node`

```text
current_node=0
```

The task is currently associated with NUMA node `0`.

A NUMA node contains one or more CPUs and their associated memory. This is not necessarily the same as the exact CPU currently executing the task.

Check the current CPU separately:

```bash
ps -p 44715 -o pid,psr,stat,pcpu,cmd
```

For all 31 threads:

```bash
ps -L -p 44715 -o pid,tid,psr,state,stat,pcpu,comm
```

## 10. Overall interpretation

The data indicates that `tsrv`:

- Has **31 threads**.
- Uses the normal `SCHED_OTHER` scheduling policy.
- Has the default nice value and normal CFS weight.
- Has accumulated about **1.944 seconds of CPU time**.
- Has accumulated about **1.228 seconds of scheduler run-queue delay**.
- Has experienced many voluntary scheduling switches and only **5 involuntary switches**.
- Has migrated between CPUs **1,684 times**.
- Has no recorded NUMA page migration or NUMA faults in this output.
- Was associated with NUMA node `0` when inspected.

For a complete per-thread review, use:

```bash
ps -L -p 44715 \
  -o pid,ppid,tid,cls,rtprio,pri,ni,psr,state,stat,pcpu,etime,wchan:24,comm
```

And inspect each thread’s detailed scheduler data under:

```bash
/proc/44715/task/<TID>/sched
```

## Practice 

- Which thread consumes more CPU resources?

```bash
[root@lnaes102 ~]# for stat in /proc/3208564/task/*/schedstat; do     tid=${stat%/*};     tid=${tid##*/};     cpu_sec=$(awk '{printf "%.6f", $1 / 1000000000}' "$stat");     printf '%-8s %12s seconds\n' "$tid" "$cpu_sec"; done | sort -k2,2nr
3208638     74.913564 seconds
3208634     74.768814 seconds
3208637     74.472010 seconds
3208633     73.499964 seconds
3208636     73.406935 seconds
3208657      7.267047 seconds
3208658      7.253702 seconds
3213968      4.778401 seconds
3208569      2.702022 seconds
3208660      2.254813 seconds
3208640      1.991574 seconds
3213964      1.936476 seconds
3208665      1.236990 seconds
3208611      0.487669 seconds
3208564      0.207844 seconds
3208577      0.166209 seconds
3208648      0.138045 seconds
3208582      0.116833 seconds
3208664      0.107116 seconds
3208655      0.013711 seconds
3208661      0.006132 seconds
3208617      0.003949 seconds
3208630      0.003250 seconds
3208663      0.001603 seconds
3208579      0.000525 seconds
3208659      0.000294 seconds
3208662      0.000146 seconds
3208610      0.000135 seconds
3208631      0.000123 seconds
3208639      0.000107 seconds
3208656      0.000068 seconds

```
- A useful command combining current CPU and thread details is:
```bash
watch -n 1 \
'ps -L -p 3208564 -o tid,psr,state,stat,pcpu,time,comm,wchan:32 --sort=-pcpu'
```
```
Every 1.0s: ps -L -p 3208564 -o tid,psr,state,stat,pcpu,time,comm,wchan:32 --sort=-pcpu                                     lnaes102: Fri Sep 11 00:48:46 2026

    TID PSR S STAT %CPU     TIME COMMAND         WCHAN
3208564   2 S Sl    0.0 00:00:00 tsrv            -
3208569   3 S Sl    0.0 00:00:03 tsrv            hrtimer_nanosleep
3208577   3 S Sl    0.0 00:00:00 tsrv            hrtimer_nanosleep
3208579   2 S Sl    0.0 00:00:00 tsrv            -
3208582   2 S Sl    0.0 00:00:00 tsrv            hrtimer_nanosleep
3208610   1 S Sl    0.0 00:00:00 tsrv            -
3208611   3 S Sl    0.0 00:00:00 tsrv            hrtimer_nanosleep
3208617   1 S Sl    0.0 00:00:00 tsrv            -
3208630   0 S Sl    0.0 00:00:00 tsrv            -
3208631   0 S Sl    0.0 00:00:00 tsrv            -
3208633   0 S Sl    0.8 00:01:23 tsrv            hrtimer_nanosleep
3208634   3 S Sl    0.8 00:01:24 tsrv            hrtimer_nanosleep
3208636   0 S Sl    0.7 00:01:23 tsrv            hrtimer_nanosleep
3208637   2 S Sl    0.8 00:01:24 tsrv            hrtimer_nanosleep
3208638   3 S Sl    0.8 00:01:24 tsrv            hrtimer_nanosleep
3208639   0 S Sl    0.0 00:00:00 tsrv            -
3208640   2 S Sl    0.0 00:00:02 tsrv            -
3208648   3 S Sl    0.0 00:00:00 tsrv            -
3208655   0 S Sl    0.0 00:00:00 tsrv            -
3208656   2 S Sl    0.0 00:00:00 tsrv            do_syscall_64
3208657   2 S Sl    0.0 00:00:08 tsrv            hrtimer_nanosleep
3208658   3 S Sl    0.0 00:00:08 tsrv            hrtimer_nanosleep
3208659   2 S Sl    0.0 00:00:00 tsrv            core_sys_select
3208660   3 S Sl    0.0 00:00:02 tsrv            -
3208661   2 S Sl    0.0 00:00:00 tsrv            -
3208662   0 S Sl    0.0 00:00:00 tsrv            -
3208663   0 S Sl    0.0 00:00:00 tsrv            core_sys_select
3208664   1 S Sl    0.0 00:00:00 tsrv            -
3208665   0 S Sl    0.0 00:00:01 tsrv            -
3213964   0 S Sl    0.0 00:00:02 tsrv            -
3213968   1 S Sl    0.0 00:00:05 tsrv            -
```

- Find which functions consume the CPU

If one of the threads is actively busy, use perf:
```bash
perf top -t 3208638
```
