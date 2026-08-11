# Course content

Refer to [Library Call](https://avaya.atlassian.net/wiki/spaces/DLBBEWIKI/pages/2448850963/1-4+MM#Library-Call) for more inforamtion about the content

We need to understand what the difference is by small and large memory allocation requests

>Under the hood, malloc() dynamically delegates tasks to the underlying system calls based on the request size:
>- Small Requests (< 128 KB): malloc() extends the process heap boundary by invoking the brk() system call.
>- Large Requests (>= 128 KB): malloc() allocates an independent virtual memory region entirely outside the heap by invoking the mmap() system call.

# Sample Program

Below is a sample program that demonstrates the typical behavior of `malloc()`:

- A small allocation is normally obtained from the process heap.
- A large allocation is normally obtained through an anonymous `mmap()` region.
- `strace` can show the underlying `brk()`, `mmap()`, and `munmap()` system calls.

> ⚠️ Important: `malloc()` does not necessarily issue one system call for every allocation. The C library, usually glibc, manages an allocator arena and may reuse memory that it already obtained from the kernel.

## Sample program

```c
#define _GNU_SOURCE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SMALL_SIZE (64 * 1024)       // 64 KiB
#define LARGE_SIZE (256 * 1024)      // 256 KiB
#define SMALL_COUNT 16

static void touch_memory(char *memory, size_t size)
{
    /*
     * Write one byte per page.
     * This makes the allocated pages actually usable and observable.
     */
    long page_size = sysconf(_SC_PAGESIZE);

    for (size_t i = 0; i < size; i += (size_t)page_size) {
        memory[i] = 'A';
    }

    memory[size - 1] = 'Z';
}

static void show_mapping_for_address(const char *name, void *address)
{
    FILE *maps = fopen("/proc/self/maps", "r");

    if (maps == NULL) {
        perror("fopen");
        return;
    }

    unsigned long target = (unsigned long)address;
    char line[512];

    while (fgets(line, sizeof(line), maps) != NULL) {
        unsigned long start;
        unsigned long end;

        if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
            if (target >= start && target < end) {
                printf("%-15s %p -> %s", name, address, line);
                break;
            }
        }
    }

    fclose(maps);
}

int main(void)
{
    
    printf("Process ID: %d\n", getpid());
    printf("\nPress Enter to start the test...\n");
    getchar();
    
    /*
     * Set the glibc mmap threshold to 128 KiB for this demonstration.
     *
     * Requests larger than this threshold will normally use mmap().
     * Requests smaller than this threshold will normally be serviced
     * from the malloc heap arena.
     */
    if (mallopt(M_MMAP_THRESHOLD, 128 * 1024) == 0) {
        fprintf(stderr, "Warning: mallopt() could not set the threshold\n");
    }

    setbuf(stdout, NULL);

    printf("Process ID: %d\n", getpid());
    printf("Page size: %ld bytes\n", sysconf(_SC_PAGESIZE));
    printf("Small allocation: %d bytes\n", SMALL_SIZE);
    printf("Large allocation: %d bytes\n\n", LARGE_SIZE);

    printf("Initial program break: %p\n", sbrk(0));

    /*
     * Allocate several small blocks.
     *
     * They will normally be obtained from the malloc heap arena.
     * Multiple malloc() calls may be satisfied by one brk() operation.
     */
    char *small_blocks[SMALL_COUNT];

    for (int i = 0; i < SMALL_COUNT; i++) {
        small_blocks[i] = malloc(SMALL_SIZE);

        if (small_blocks[i] == NULL) {
            perror("malloc small block");
            return EXIT_FAILURE;
        }

        touch_memory(small_blocks[i], SMALL_SIZE);

        printf("Allocated %d x %d bytes\n", i+1, SMALL_SIZE);
        printf("Program break: %p\n", sbrk(0));
    }

    printf("\nAfter small allocations:\n");
    printf("Program break: %p\n", sbrk(0));

    show_mapping_for_address("small_blocks[0]", small_blocks[0]);

    /*
     * Allocate a large block.
     *
     * With the configured threshold, this will normally be allocated
     * from an anonymous mmap() region rather than the [heap] mapping.
     */
    char *large_block = malloc(LARGE_SIZE);

    if (large_block == NULL) {
        perror("malloc large block");
        return EXIT_FAILURE;
    }

    touch_memory(large_block, LARGE_SIZE);

    printf("\nAfter large allocation:\n");
    printf("Program break: %p\n", sbrk(0));

    show_mapping_for_address("large_block", large_block);

    printf("\nPress Enter to free the memory and exit...\n");
    getchar();

    /*
     * The large mmap-backed allocation will normally be returned using
     * munmap(), while small allocations are returned to the malloc arena.
     */
    free(large_block);

    for (int i = 0; i < SMALL_COUNT; i++) {
        free(small_blocks[i]);
    }

    printf("Memory freed.\n");

    return EXIT_SUCCESS;
}
```

## Compile

```bash
gcc -Wall -Wextra -O0 -g malloc_demo.c -o malloc_demo
```

## Run normally

```bash
./malloc_demo
```

Example output may look like:

```text
[root@DA22053700:LinuxKernel-1-4-MM]# ./malloc_demo
Process ID: 134394

Press Enter to start the test...

Process ID: 134394
Page size: 4096 bytes
Small allocation: 65536 bytes
Large allocation: 262144 bytes

Initial program break: 0x621820674000
Allocated 1 x 65536 bytes
Program break: 0x621820674000
Allocated 2 x 65536 bytes
Program break: 0x621820674000
Allocated 3 x 65536 bytes
Program break: 0x6218206a4000
Allocated 4 x 65536 bytes
Program break: 0x6218206a4000
Allocated 5 x 65536 bytes
Program break: 0x6218206a4000
Allocated 6 x 65536 bytes
Program break: 0x6218206d4000
Allocated 7 x 65536 bytes
Program break: 0x6218206d4000
Allocated 8 x 65536 bytes
Program break: 0x6218206d4000
Allocated 9 x 65536 bytes
Program break: 0x621820704000
Allocated 10 x 65536 bytes
Program break: 0x621820704000
Allocated 11 x 65536 bytes
Program break: 0x621820704000
Allocated 12 x 65536 bytes
Program break: 0x621820734000
Allocated 13 x 65536 bytes
Program break: 0x621820734000
Allocated 14 x 65536 bytes
Program break: 0x621820734000
Allocated 15 x 65536 bytes
Program break: 0x621820764000
Allocated 16 x 65536 bytes
Program break: 0x621820764000

After small allocations:
Program break: 0x621820764000
small_blocks[0] 0x621820653ac0 -> 621820653000-621820764000 rw-p 00000000 00:00 0                          [heap]
Small allocation completed, press Enter Key to continue

After large allocation:
Program break: 0x621820764000
large_block     0x7e2a82960010 -> 7e2a82960000-7e2a829a4000 rw-p 00000000 00:00 0

Press Enter to free the memory and exit...

```

Meanwhile, we have below observation from /proc virtual file maps, before allocation started
```
[root@DA22053700:134394]# cat maps
6217f9641000-6217f9642000 r--p 00000000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9642000-6217f9643000 r-xp 00001000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9643000-6217f9644000 r--p 00002000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9644000-6217f9645000 r--p 00002000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9645000-6217f9646000 rw-p 00003000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
621820653000-621820674000 rw-p 00000000 00:00 0                          [heap]
```

After allocation completed
```
[root@DA22053700:134394]# cat maps
6217f9641000-6217f9642000 r--p 00000000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9642000-6217f9643000 r-xp 00001000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9643000-6217f9644000 r--p 00002000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9644000-6217f9645000 r--p 00002000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9645000-6217f9646000 rw-p 00003000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
621820653000-621820764000 rw-p 00000000 00:00 0                          [heap]
7e2a82600000-7e2a82628000 r--p 00000000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a82628000-7e2a827bd000 r-xp 00028000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a827bd000-7e2a82815000 r--p 001bd000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a82815000-7e2a82816000 ---p 00215000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a82816000-7e2a8281a000 r--p 00215000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a8281a000-7e2a8281c000 rw-p 00219000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a8281c000-7e2a82829000 rw-p 00000000 00:00 0
7e2a829a1000-7e2a829a4000 rw-p 00000000 00:00 0
7e2a829ae000-7e2a829b0000 rw-p 00000000 00:00 0
7e2a829b0000-7e2a829b4000 r--p 00000000 00:00 0                          [vvar]
7e2a829b4000-7e2a829b6000 r--p 00000000 00:00 0                          [vvar_vclock]
7e2a829b6000-7e2a829b8000 r-xp 00000000 00:00 0                          [vdso]
7e2a829b8000-7e2a829ba000 r--p 00000000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829ba000-7e2a829e4000 r-xp 00002000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829e4000-7e2a829ef000 r--p 0002c000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829f0000-7e2a829f2000 r--p 00037000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829f2000-7e2a829f4000 rw-p 00039000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffe6d679000-7ffe6d69b000 rw-p 00000000 00:00 0                          [stack]
```

After large allocation completed
```
[root@DA22053700:134394]# cat maps
6217f9641000-6217f9642000 r--p 00000000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9642000-6217f9643000 r-xp 00001000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9643000-6217f9644000 r--p 00002000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9644000-6217f9645000 r--p 00002000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
6217f9645000-6217f9646000 rw-p 00003000 00:47 4785074604191529           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/malloc_demo
621820653000-621820764000 rw-p 00000000 00:00 0                          [heap]
7e2a82600000-7e2a82628000 r--p 00000000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a82628000-7e2a827bd000 r-xp 00028000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a827bd000-7e2a82815000 r--p 001bd000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a82815000-7e2a82816000 ---p 00215000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a82816000-7e2a8281a000 r--p 00215000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a8281a000-7e2a8281c000 rw-p 00219000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
7e2a8281c000-7e2a82829000 rw-p 00000000 00:00 0
7e2a82960000-7e2a829a4000 rw-p 00000000 00:00 0
7e2a829ae000-7e2a829b0000 rw-p 00000000 00:00 0
7e2a829b0000-7e2a829b4000 r--p 00000000 00:00 0                          [vvar]
7e2a829b4000-7e2a829b6000 r--p 00000000 00:00 0                          [vvar_vclock]
7e2a829b6000-7e2a829b8000 r-xp 00000000 00:00 0                          [vdso]
7e2a829b8000-7e2a829ba000 r--p 00000000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829ba000-7e2a829e4000 r-xp 00002000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829e4000-7e2a829ef000 r--p 0002c000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829f0000-7e2a829f2000 r--p 00037000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7e2a829f2000-7e2a829f4000 rw-p 00039000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffe6d679000-7ffe6d69b000 rw-p 00000000 00:00 0                          [stack]
```

### Conclusion of small allocations
- The first 2 allocations occurred with the initial heap - which was 
```
0x621820674000 - 0x621820653000
= 0x21000 bytes
= 135168 bytes
```
- each request is:
````
65536 bytes = 0x10000 bytes
````
- The first two allocations require:
```
2 × 65536 = 131072 bytes = 0x20000 bytes
```
The allocator already has enough usable free space in its current heap area to satisfy these requests.

- The third request needs another 65536 bytes. At this point, the remaining free space is insufficient for the requested chunk plus allocator metadata and alignment padding. glibc therefore extends the heap using brk().

The program break changes from:
```
0x621820674000
```
to:
```
0x6218206a4000
```
The increase is:
```
0x6218206a4000 - 0x621820674000
= 0x30000
= 196608 bytes
= 192 KiB
```
glibc does this deliberately. It obtains additional space for future allocations instead of asking the kernel for exactly 64 KiB every time.

After the third allocation, the heap conceptually looks like:

```
┌────────────┬──────┬──────┬──────┬────────────────────────┐
│ metadata   │ p1   │ p2   │ p3   │ remaining free space   │
└────────────┴──────┴──────┴──────┴────────────────────────┘
                                                    ▲
                                                    │
                                             new program break
```
- The next two allocations fit into that newly acquired free space. and so forth until the last allocation completed.

### Conclusion of large allocation
- The kernel manages virtual memory using VMAs—Virtual Memory Areas. Each line in /proc/\<pid\>/maps represents a VMA or a merged range of compatible VMAs.
- Before the allocation, there was a mapping:
```
7e2a829a1000-7e2a829a4000 rw-p
```
- A new anonymous writable mapping was created immediately before it:
```
7e2a82960000-7e2a829a1000 rw-p
```
- Because the two ranges were adjacent and had compatible properties, Linux could represent them as one combined VMA:
```
7e2a82960000-7e2a829a4000 rw-p
```
- Conceptually
```
Before:

7e2a82960000                         7e2a829a1000       7e2a829a4000
       unused/gap                            ┌──────────────┐
                                            │ existing VMA │
                                            └──────────────┘


After mmap():

7e2a82960000       7e2a829a1000                         7e2a829a4000
       ┌──────────────┬──────────────────────────────────┐
       │ new mapping  │ existing anonymous mapping       │
       └──────────────┴──────────────────────────────────┘
       <──────────── one merged VMA ─────────────────────>
```

- The large allocation is therefore not part of the traditional [heap] area. It is an independent anonymous virtual-memory mapping.




## Trace the system calls with `strace`

Run:

```bash
strace -f -e trace=brk,mmap,munmap ./malloc_demo
```

You may see output similar to:

```text
[root@DA22053700:LinuxKernel-1-4-MM]# strace -e trace=brk,mmap,munmap ./malloc_demo
brk(NULL)                               = 0x55acc250c000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7888c1a55000
mmap(NULL, 37304, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7888c1a4b000
mmap(NULL, 2264656, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7888c1800000
mmap(0x7888c1828000, 1658880, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7888c1828000
mmap(0x7888c19bd000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1bd000) = 0x7888c19bd000
mmap(0x7888c1a16000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x215000) = 0x7888c1a16000
mmap(0x7888c1a1c000, 52816, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7888c1a1c000
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7888c1a48000
munmap(0x7888c1a4b000, 37304)           = 0
brk(NULL)                               = 0x55acc250c000
brk(0x55acc252d000)                     = 0x55acc252d000
Process ID: 141011

Press Enter to start the test...

Process ID: 141011
Page size: 4096 bytes
Small allocation: 65536 bytes
Large allocation: 262144 bytes

Initial program break: 0x55acc252d000
Allocated 1 x 65536 bytes
Program break: 0x55acc252d000
Allocated 2 x 65536 bytes
Program break: 0x55acc252d000
brk(0x55acc255d000)                     = 0x55acc255d000
Allocated 3 x 65536 bytes
Program break: 0x55acc255d000
Allocated 4 x 65536 bytes
Program break: 0x55acc255d000
Allocated 5 x 65536 bytes
Program break: 0x55acc255d000
brk(0x55acc258d000)                     = 0x55acc258d000
Allocated 6 x 65536 bytes
Program break: 0x55acc258d000
Allocated 7 x 65536 bytes
Program break: 0x55acc258d000
Allocated 8 x 65536 bytes
Program break: 0x55acc258d000
brk(0x55acc25bd000)                     = 0x55acc25bd000
Allocated 9 x 65536 bytes
Program break: 0x55acc25bd000
Allocated 10 x 65536 bytes
Program break: 0x55acc25bd000
Allocated 11 x 65536 bytes
Program break: 0x55acc25bd000
brk(0x55acc25ed000)                     = 0x55acc25ed000
Allocated 12 x 65536 bytes
Program break: 0x55acc25ed000
Allocated 13 x 65536 bytes
Program break: 0x55acc25ed000
Allocated 14 x 65536 bytes
Program break: 0x55acc25ed000
brk(0x55acc261d000)                     = 0x55acc261d000
Allocated 15 x 65536 bytes
Program break: 0x55acc261d000
Allocated 16 x 65536 bytes
Program break: 0x55acc261d000

After small allocations:
Program break: 0x55acc261d000
small_blocks[0] 0x55acc250cac0 -> 55acc250c000-55acc261d000 rw-p 00000000 00:00 0                          [heap]
Small allocation completed, press Enter Key to continue

mmap(NULL, 266240, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7888c17bf000

After large allocation:
Program break: 0x55acc261d000
large_block     0x7888c17bf010 -> 7888c17bf000-7888c1800000 rw-p 00000000 00:00 0

Press Enter to free the memory and exit...

munmap(0x7888c17bf000, 266240)          = 0
Memory freed.
+++ exited with 0 +++
```

The exact sizes and addresses will vary.

## What the calls mean

### Small allocations

```c
malloc(64 * 1024);
```

Normally:

1. glibc checks whether it already has a suitable free block.
2. If not, it expands its heap arena.
3. The arena may be expanded using:

```text
brk()
```

However, several small allocations can be served by one `brk()` call.

### Large allocations

```c
malloc(256 * 1024);
```

With a 128 KiB threshold, glibc will normally use:

```text
mmap()
```

The resulting memory is usually outside the `[heap]` mapping.

When the memory is released:

```c
free(large_block);
```

glibc can normally return the mapping directly to the kernel using:

```text
munmap()
```


## Important qualifications

The statement:

```text
Small requests use brk()
Large requests use mmap()
```

is a useful simplified model, but it is not an absolute rule.

Actual behavior depends on:

- glibc version
- Allocation size
- Current allocator state
- Existing free blocks
- Thread arenas
- The current `mmap` threshold
- Memory alignment
- Allocator tunables
- Whether the allocation is subsequently reused

Also, the threshold is about the allocator's internal decision, not a kernel rule. The kernel provides `brk()` and `mmap()`; glibc's `malloc()` decides which one to use.
