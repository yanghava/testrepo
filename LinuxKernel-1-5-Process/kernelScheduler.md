To review Linux’s kernel scheduling clock, inspect the **clocksource**, kernel tick frequency, and `sched_clock()` behavior.

## 1. Check the active kernel clocksource

```bash
[root@lnaes102 3208564]# cat /sys/devices/system/clocksource/clocksource0/current_clocksource
tsc

[root@lnaes102 3208564]# cat /sys/devices/system/clocksource/clocksource0/available_clocksource
tsc hpet acpi_pm
```

`tsc` is commonly used on modern x86 systems.

Check the kernel boot log:

```bash
dmesg | grep -iE 'clocksource|sched_clock|clock tick|tsc'
```

or:

```bash
journalctl -k | grep -iE 'clocksource|sched_clock|clock tick|tsc'
```

## 2. Check kernel tick frequency (`CONFIG_HZ`)

```bash
zgrep CONFIG_HZ /proc/config.gz 2>/dev/null
```

If `/proc/config.gz` is unavailable:

```bash
grep CONFIG_HZ /boot/config-$(uname -r)
```

Typical values:

```text
CONFIG_HZ_250=y
CONFIG_HZ=250
```

What is configured on an AES server

```bash
[root@lnaes102 ~]# zgrep CONFIG_HZ /proc/config.gz 2>/dev/null
[root@lnaes102 ~]# grep CONFIG_HZ /boot/config-$(uname -r)
# CONFIG_HZ_PERIODIC is not set
# CONFIG_HZ_100 is not set
# CONFIG_HZ_250 is not set
# CONFIG_HZ_300 is not set
CONFIG_HZ_1000=y
CONFIG_HZ=1000

```

Common settings are `100`, `250`, and `1000`. This is the kernel timer tick frequency, not necessarily the resolution of `sched_clock()`.

You can also check the configured timer frequency from:

```bash
getconf CLK_TCK
```

Usually this returns `100`, but it represents userspace jiffy accounting and does not directly identify the kernel clocksource.
```bash
[root@lnaes102 ~]# getconf CLK_TCK
100
```


## 3. Inspect timer and clock details

As root:

```bash
sudo cat /proc/timer_list
```

This can show per-CPU timer bases, tick devices, clock event devices, and timer configuration. On some kernels, `/proc/timer_list` may be disabled or restricted.

Useful excerpts:

```bash
sudo cat /proc/timer_list | grep -E 'now at|tick_nsec|clock .*device|resolution'
```

## 4. Compare userspace clock readings

```bash
date +%s.%N
```

For monotonic clocks:

```bash
python3 - <<'PY'
import time

for name, clock in [
    ("monotonic", time.CLOCK_MONOTONIC),
    ("monotonic_raw", time.CLOCK_MONOTONIC_RAW),
    ("boottime", time.CLOCK_BOOTTIME),
    ("realtime", time.CLOCK_REALTIME),
]:
    print(f"{name:15} {time.clock_gettime_ns(clock)} ns")
PY
```

`CLOCK_MONOTONIC_RAW` is useful for observing the underlying hardware-based progression without NTP frequency adjustments. It is not identical to the kernel’s internal `sched_clock()` implementation.

## 5. Observe scheduler timestamps with ftrace

Mount and enable tracing:

```bash
sudo mount -t debugfs none /sys/kernel/debug 2>/dev/null || true

cd /sys/kernel/debug/tracing
sudo sh -c 'echo 1 > tracing_on'
sudo sh -c 'echo 1 > events/sched/sched_switch/enable'
sudo sh -c 'echo 1 > events/sched/sched_wakeup/enable'
sleep 1
sudo sh -c 'echo 0 > tracing_on'
sudo cat trace
```

The trace timestamps are based on the kernel tracing clock. Check or change it with:

```bash
cat /sys/kernel/debug/tracing/trace_clock
```

Example output:

```text
local global counter mono mono_raw boot x86-tsc
```

Select a clock, if supported:

```bash
echo mono_raw | sudo tee /sys/kernel/debug/tracing/trace_clock
```

Then collect scheduler events:

```bash
sudo trace-cmd record -e sched_switch -e sched_wakeup sleep 5
sudo trace-cmd report
```

Install `trace-cmd` through your distribution’s package manager if necessary.

## 6. Check the tracing clock from the kernel

The kernel’s `sched_clock()` is primarily an internal API. A small kernel module or eBPF program is needed to call it directly. With BPF tracing, you can inspect scheduler events and their timestamps, but ordinary shell commands generally cannot directly read `sched_clock()`.

For scheduler latency analysis, use:

```bash
sudo perf sched timehist
```

or:

```bash
sudo perf sched record -- sleep 5
sudo perf sched latency
```

## 7. Check boot parameters affecting scheduling clocks

```bash
cat /proc/cmdline
```

Look for parameters such as:

```text
clocksource=
tsc=
nohz=
nohz_full=
highres=
```

Also check whether high-resolution timers are enabled:

```bash
zgrep CONFIG_HIGH_RES_TIMERS /proc/config.gz 2>/dev/null
grep CONFIG_HIGH_RES_TIMERS /boot/config-$(uname -r)
```

### Quick summary

```bash
uname -a
cat /sys/devices/system/clocksource/clocksource0/current_clocksource
cat /sys/devices/system/clocksource/clocksource0/available_clocksource
grep CONFIG_HZ /boot/config-$(uname -r)
cat /sys/kernel/debug/tracing/trace_clock
dmesg | grep -iE 'clocksource|sched_clock|tsc'
```

The most important distinction is:

- **Clocksource**: hardware/software source used for timekeeping.
- **`CONFIG_HZ`**: periodic kernel tick frequency.
- **`sched_clock()`**: fast internal timestamp source used for scheduler accounting and tracing.
- **POSIX clocks** such as `CLOCK_MONOTONIC`: userspace-visible time interfaces.