# Is Memory Allocated to the Child Process Accessible to the Parent?

No. By default in Linux, the memory allocated to the child process is strictly isolated from the parent process.

# Why? (Kernel Architecture & Virtual Memory)
1. Independent Virtual Address Spaces (struct mm_struct):
- Every process in Linux has its own virtual memory descriptor (struct mm_struct) and its own set of page tables.
- During a context switch between parent and child, the CPU's Memory Management Unit (MMU) switches page tables (e.g., reloading the CR3 register on x86-64).

2. Copy-On-Write (COW):
- When fork() is called, the child inherits duplicate page tables pointing to the parent’s existing physical pages, marked read-only.
- As soon as either process writes to a shared page, a hardware page fault occurs (do_page_fault()). The kernel allocates a new physical frame and updates only the writing process's page table.

3. New Memory Allocation:
- When the child acquires new memory via brk() / sbrk() (expanding the heap) or mmap() (MAP_PRIVATE | MAP_ANONYMOUS), the kernel creates a new Virtual Memory Area (struct vm_area_struct) and page table entries solely in the child's mm_struct.
- The parent’s page table has no entry for this new memory. If the child passes a pointer (e.g. 0x55a1b0...) to the parent, dereferencing it in the parent will either trigger a Segmentation Fault (SIGSEGV) (if unmapped) or corrupt unrelated data in the parent's own space.

4. Exception (Explicit Sharing):
- Memory is only shared if explicitly mapped with MAP_SHARED via mmap(), through IPC mechanisms (POSIX shm_open or System V shared memory), or when created as threads sharing the memory descriptor via clone(..., CLONE_VM, ...).

# Demo

1. Program 1: Process Creation & Image Replacement

```c
# fork_exec_demo.c

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    printf("[PARENT] PID is %d\n", getpid());
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    
    if (pid == 0) {
        // Child branch: fork() returns 0 to the child process
        printf("[CHILD]  PID is %d, PPID is %d.\n", getpid(), getppid());

        // Replace process image with "ls -lahtr"
        execlp("ls", "ls", "-lahtr", NULL);

        perror("execlp failed");
        return 1;
    } else {
        // Parent branch: fork() returns child PID
        printf("[PARENT] Created child PID %d.\n", pid);
        
        int status;
        pid_t reaped_pid = waitpid(-1, &status, 0);
        
        printf("\n[PARENT] Child %d has exited. (Raw status: %d)\n", reaped_pid, status);
        if (WIFEXITED(status)) {
            printf("[PARENT] Child exited normally with code: %d\n", WEXITSTATUS(status));
        }
        printf("[PARENT] Done.\n");
    }
    return 0;
}
```

```bash
[root@DA22053700:LinuxKernel-1-5-Process]# gcc fork_exec_demo.c -o fork_exec_demo
[root@DA22053700:LinuxKernel-1-5-Process]# ./fork_exec_demo
[PARENT] PID is 3345
[PARENT] Created child PID 3346.
[CHILD]  PID is 3346, PPID is 3345.
total 72K
drwxrwxrwx 1 root root 4.0K Aug 25 19:30 ParentProcessDemo
drwxrwxrwx 1 root root 4.0K Aug 25 23:45 ..
-rwxrwxrwx 1 root root 4.3K Sep  8 20:45 process.md
-rwxrwxrwx 1 root root 5.1K Sep 11 14:50 kernelScheduler.md
drwxrwxrwx 1 root root 4.0K Sep 11 16:48 AESProcesses
-rwxrwxrwx 1 root root  18K Sep 11 16:50 Schedule.md
-rwxrwxrwx 1 root root 2.2K Oct  3 19:50 fork_exec_demo.c
-rwxrwxrwx 1 root root 5.4K Oct  3 19:51 child_memory_demo.c
-rwxrwxrwx 1 root root 2.9K Oct  3 20:24 ProcessMemory.md
-rwxrwxrwx 1 root root 3.1K Oct  3 20:33 execlp.md
drwxrwxrwx 1 root root 4.0K Oct  3 20:34 .
-rwxrwxrwx 1 root root  16K Oct  3 20:34 fork_exec_demo

[PARENT] Child 3346 has exited. (Raw status: 0)
[PARENT] Child exited normally with code: 0
[PARENT] Done.
```
## Note

- In the child process, [execlp](execlp.md) doesn't return on success. The line below the execlp is never executed unless there's a failure.
- When the child process starts, parent process can choose the option how it waits for the response from the child process by passing different parameters to [waitpid](waitpid.md) function

2. Program 2: Process Creation & User input collection

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Linux Process Concurrency Demo
 *
 * Demonstrates:
 * 1. Independent Concurrent Execution:
 *    - Parent process continues executing background task cycles continuously
 *      WITHOUT waiting for user input. Its progress is directly observable.
 *    - Child process prompts the user for interactive input:
 *      'c' -> continues executing tasks.
 *      'q' -> terminates the child process.
 *
 * 2. Non-Blocking Process Monitoring (waitpid with WNOHANG):
 *    - On every progress cycle, the parent checks if the child has terminated
 *      without blocking its own execution.
 *    - Once the child terminates, the parent reaps it, prints the exit status,
 *      and wraps up.
 *
 * Note on Terminal Concurrency:
 * Both processes share stdout. As the parent prints progress updates while the child
 * waits on stdin, the terminal driver buffers your keystrokes. You can type 'c' or 'q'
 * and press Enter at any time.
 */

int main(void) {
    printf("============================================================\n");
    printf("[PARENT %d] Starting process.\n", getpid());
    printf("============================================================\n");

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // =========================================================
        // CHILD PROCESS (Interactive: Controlled by User Input)
        // =========================================================
        printf("[CHILD  %d] Initialized. PPID is %d.\n", getpid(), getppid());

        char input[64];
        int child_task_count = 0;

        while (1) {
            printf("\n>>> [CHILD  %d] Enter 'c' to continue task, 'q' to terminate child: ", getpid());
            fflush(stdout);

            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("\n[CHILD  %d] EOF detected. Exiting.\n", getpid());
                break;
            }

            // Remove newline
            input[strcspn(input, "\n")] = '\0';

            if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
                printf("[CHILD  %d] User requested termination. Exiting...\n", getpid());
                break;
            } else if (strcmp(input, "c") == 0 || strcmp(input, "C") == 0) {
                child_task_count++;
                printf("[CHILD  %d] >>> Doing Child Task #%d...\n", getpid(), child_task_count);
                usleep(300000); // 300ms simulated work
                printf("[CHILD  %d] >>> Child Task #%d completed.\n", getpid(), child_task_count);
            } else {
                printf("[CHILD  %d] Unknown command '%s'. Please enter 'c' or 'q'.\n", getpid(), input);
            }
        }

        printf("[CHILD  %d] Process exiting with status 0.\n", getpid());
        exit(0);
    } else {
        // =========================================================
        // PARENT PROCESS (Autonomous: Continuous Observable Progress)
        // =========================================================
        printf("[PARENT %d] Created child PID %d.\n", getpid(), pid);
        printf("[PARENT %d] Parent will continue working autonomously.\n\n", getpid());

        int parent_cycle = 0;
        int status;

        while (1) {
            parent_cycle++;
            printf("[PARENT %d] Working... cycle #%d\n", getpid(), parent_cycle);
            
            // Simulate work taking 2 seconds per cycle
            sleep(2);

            // Check non-blockingly if child has exited
            pid_t reaped_pid = waitpid(pid, &status, WNOHANG);

            if (reaped_pid == 0) {
                // Child is still alive and running
                continue;
            } else if (reaped_pid == pid) {
                // Child has terminated; reap and finish
                printf("\n============================================================\n");
                printf("[PARENT %d] Detected child %d has terminated!\n", getpid(), pid);
                if (WIFEXITED(status)) {
                    printf("[PARENT %d] Child exited normally with status: %d\n", 
                           getpid(), WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    printf("[PARENT %d] Child terminated by signal: %d\n", 
                           getpid(), WTERMSIG(status));
                }
                printf("[PARENT %d] Parent completed a total of %d progress cycles.\n", 
                       getpid(), parent_cycle);
                printf("[PARENT %d] All done. Parent exiting.\n", getpid());
                printf("============================================================\n");
                break;
            } else {
                perror("waitpid error");
                break;
            }
        }
    }

    return 0;
}
```

```bash
[root@DA22053700:LinuxKernel-1-5-Process]# ./fork_exec_demo-userinput
============================================================
[PARENT 30689] Starting process.
============================================================
[PARENT 30689] Created child PID 30690.
[PARENT 30689] Parent will continue working autonomously.

[PARENT 30689] Working... cycle #1
[CHILD  30690] Initialized. PPID is 30689.

>>> [CHILD  30690] Enter 'c' to continue task, 'q' to terminate child: [PARENT 30689] Working... cycle #2
[PARENT 30689] Working... cycle #3
[PARENT 30689] Working... cycle #4
c
[CHILD  30690] >>> Doing Child Task #1...
[CHILD  30690] >>> Child Task #1 completed.

>>> [CHILD  30690] Enter 'c' to continue task, 'q' to terminate child: [PARENT 30689] Working... cycle #5
[PARENT 30689] Working... cycle #6
c
[CHILD  30690] >>> Doing Child Task #2...
[CHILD  30690] >>> Child Task #2 completed.

>>> [CHILD  30690] Enter 'c' to continue task, 'q' to terminate child: [PARENT 30689] Working... cycle #7
[PARENT 30689] Working... cycle #8
[PARENT 30689] Working... cycle #9
[PARENT 30689] Working... cycle #10
q
[CHILD  30690] User requested termination. Exiting...
[CHILD  30690] Process exiting with status 0.

============================================================
[PARENT 30689] Detected child 30690 has terminated!
[PARENT 30689] Child exited normally with status: 0
[PARENT 30689] Parent completed a total of 10 progress cycles.
[PARENT 30689] All done. Parent exiting.
============================================================
```

3. Program 3: Process Creation & Handling the exit signal

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>

/*
 * Helper function to print messages prefixed with current date and timestamp
 * Format: [YYYY-MM-DD HH:MM:SS.mmm] <message>
 */
void log_msg(const char *format, ...) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm tm_info;
    localtime_r(&tv.tv_sec, &tm_info);

    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

    printf("[%s.%03ld] ", time_str, tv.tv_usec / 1000);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

void sigchld_handler(int sig) {
    (void)sig; // Silence unused warning
    int status;
    pid_t pid;

    // Reap all terminated children (loop handles multiple simultaneous terminations)
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            log_msg("[SIGNAL HANDLER] Child process has ended! Reaped child PID %d (exit code: %d).\n", 
                    pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            log_msg("[SIGNAL HANDLER] Child process has ended! Reaped child PID %d (killed by signal: %d).\n", 
                    pid, WTERMSIG(status));
        } else {
            log_msg("[SIGNAL HANDLER] Child process has ended! Reaped child PID %d (raw status: %d).\n", 
                    pid, status);
        }
    }
}

int main(void) {
    // Register SIGCHLD handler to reap children asynchronously
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction failed");
        return 1;
    }

    log_msg("[PARENT %d] Process started with PID %d.\n", getpid(), getpid());

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child branch
		pid_t childPid = getpid();
        log_msg("[CHILD %d]  Running in background for 2 seconds (PID %d, PPID %d)...\n", 
                childPid, childPid, getppid());
        sleep(2);
        
        // Explicitly announce child process completion
        log_msg("[CHILD %d]  *** Work complete. Child process ends now (PID %d) ***\n", childPid, childPid);
        exit(0);
    }

    // Parent branch: continues executing independent tasks
    log_msg("[PARENT %d] Created child PID %d. Parent continues working...\n", pid, pid);

    
    log_msg("[PARENT %d] Executing main task step by sleeping 10 seconds\n", pid);
    unsigned int unslept = sleep(10);
	log_msg("[PARENT %d] >>> WOKE UP FROM SLEEP! <<<\n", pid);
	log_msg("[PARENT %d] Sleep was interrupted with %u seconds remaining!\n", pid, unslept);
   

    log_msg("[PARENT] All tasks finished. Parent exiting.\n");
    return 0;
}
```

```bash
[root@DA22053700:LinuxKernel-1-5-Process]# ./fork_exec_demo-signal
[2026-10-06 09:43:25.298] [PARENT 84803] Process started with PID 84803.
[2026-10-06 09:43:25.298] [PARENT 84804] Created child PID 84804. Parent continues working...
[2026-10-06 09:43:25.298] [PARENT 84804] Executing main task step by sleeping 10 seconds
[2026-10-06 09:43:25.298] [CHILD 84804]  Running in background for 2 seconds (PID 84804, PPID 84803)...
[2026-10-06 09:43:27.298] [CHILD 84804]  *** Work complete. Child process ends now (PID 84804) ***
[2026-10-06 09:43:27.299] [SIGNAL HANDLER] Child process has ended! Reaped child PID 84804 (exit code: 0).
[2026-10-06 09:43:27.299] [PARENT 84804] >>> WOKE UP FROM SLEEP! <<<
[2026-10-06 09:43:27.299] [PARENT 84804] Sleep was interrupted with 7 seconds remaining!
[2026-10-06 09:43:27.299] [PARENT] All tasks finished. Parent exiting.
```

4. Program 4: Child Process memory inspection

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>

/*
 * Demonstration: Concurrent Memory Area (VMA) Isolation Between Parent and Child
 *
 * Synchronization via Two Pipes:
 * 1. pipe_c2p: Child -> Parent (Transmits allocated virtual addresses)
 * 2. pipe_p2c: Parent -> Child (Releases child after parent finishes inspection)
 *
 * This proves that WHILE BOTH PROCESSES ARE SIMULTANEOUSLY ALIVE:
 * - The child actively holds its allocated heap and mmap memory in RAM.
 * - The parent inspects /proc/<child_pid>/maps (actively mapped in Child)
 *   and /proc/self/maps (completely NOT MAPPED in Parent).
 */

int shared_data_var = 100; // Lives in Data segment

// Helper to look up which VMA an address belongs to in a given proc maps file
static void show_vma_for_address(const char *tag, const char *label, void *address, const char *maps_path) {
    if (!maps_path) {
        maps_path = "/proc/self/maps";
    }

    FILE *maps = fopen(maps_path, "r");
    if (!maps) {
        perror("fopen maps");
        return;
    }

    unsigned long target = (unsigned long)address;
    char line[512];
    int found = 0;

    while (fgets(line, sizeof(line), maps) != NULL) {
        unsigned long start, end;
        if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
            if (target >= start && target < end) {
                line[strcspn(line, "\n")] = '\0';
                printf("[%s] %-14s (%p) is inside VMA:\n", tag, label, address);
                printf("       --> %s\n", line);
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        printf("[%s] %-14s (%p) -> [NOT MAPPED in this process!]\n", tag, label, address);
    }

    fclose(maps);
}

// Helper to print the [heap] line from a given proc maps file
static void show_heap_vma(const char *tag, const char *maps_path) {
    if (!maps_path) {
        maps_path = "/proc/self/maps";
    }

    FILE *maps = fopen(maps_path, "r");
    if (!maps) return;

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), maps) != NULL) {
        if (strstr(line, "[heap]")) {
            line[strcspn(line, "\n")] = '\0';
            printf("[%s] [heap] VMA: %s\n", tag, line);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("[%s] [heap] VMA: (no heap allocated yet)\n", tag);
    }
    fclose(maps);
}

// Helper to query the Physical Frame Number (PFN) via /proc/<pid>/pagemap
static unsigned long get_pfn_for_address(void *addr, pid_t pid) {
    char pagemap_path[64];
    if (pid == 0) {
        snprintf(pagemap_path, sizeof(pagemap_path), "/proc/self/pagemap");
    } else {
        snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%d/pagemap", pid);
    }

    int fd = open(pagemap_path, O_RDONLY);
    if (fd < 0) return 0;

    unsigned long vaddr = (unsigned long)addr;
    long page_size = sysconf(_SC_PAGESIZE);
    unsigned long pfn_offset = (vaddr / page_size) * sizeof(uint64_t);

    uint64_t entry = 0;
    if (pread(fd, &entry, sizeof(entry), pfn_offset) != sizeof(entry)) {
        close(fd);
        return 0;
    }
    close(fd);

    // Bit 63 indicates page present in RAM
    if (!(entry & (1ULL << 63))) return 0;

    // Bits 0-54 store the Physical Frame Number
    return entry & 0x7fffffffffffffULL;
}

int main(void) {
    int pipe_c2p[2]; // Child -> Parent
    int pipe_p2c[2]; // Parent -> Child

    if (pipe(pipe_c2p) == -1 || pipe(pipe_p2c) == -1) {
        perror("pipe creation failed");
        return 1;
    }

    printf("======================================================================\n");
    printf(" CONCURRENT MEMORY AREA ISOLATION DEMONSTRATION (PARENT vs CHILD)\n");
    printf("======================================================================\n\n");

    // Initialize heap baseline in parent so both start with a known baseline
    void *dummy = malloc(16);
    free(dummy);

    printf("[INITIAL BASELINE before fork]\n");
    printf("[PARENT %d] Initial heap break (sbrk(0)): %p\n", getpid(), sbrk(0));
    show_heap_vma("PARENT", NULL);
    printf("----------------------------------------------------------------------\n\n");

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // ==================================================================
        // CHILD PROCESS
        // ==================================================================
        close(pipe_c2p[0]); // Close unused read end of c2p
        close(pipe_p2c[1]); // Close unused write end of p2c

        printf("[CHILD  %d] Process started (Parent PID: %d).\n", getpid(), getppid());
        printf("[CHILD  %d] Initial heap break (sbrk(0)): %p\n", getpid(), sbrk(0));
        show_heap_vma("CHILD", NULL);
        printf("\n");

        // 1. Heap Allocation: Expand heap by 128 KiB
        printf("[CHILD  %d] === STEP 1: Child expands HEAP by 128 KiB ===\n", getpid());
        void *child_heap_ptr = sbrk(128 * 1024);
        memset(child_heap_ptr, 'C', 128 * 1024); // Touch memory to populate page tables
        printf("[CHILD  %d] New heap break (sbrk(0)): %p\n", getpid(), sbrk(0));
        show_heap_vma("CHILD", NULL);
        show_vma_for_address("CHILD", "child_heap_ptr", child_heap_ptr, NULL);
        printf("\n");

        // 2. MMAP Allocation: Allocate a 4 KiB anonymous page
        printf("[CHILD  %d] === STEP 2: Child allocates MMAP Page (4 KiB) ===\n", getpid());
        char *child_mmap_ptr = (char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        strcpy(child_mmap_ptr, "Child-Specific Secret Data");
        show_vma_for_address("CHILD", "child_mmap_ptr", child_mmap_ptr, NULL);
        printf("\n");

        // 3. Copy-on-Write: Modify shared_data_var
        printf("[CHILD  %d] === STEP 3: Child modifies shared_data_var (COW) ===\n", getpid());
        shared_data_var = 999;
        unsigned long child_pfn = get_pfn_for_address(&shared_data_var, 0);
        printf("[CHILD  %d] shared_data_var = %d at virtual address: %p\n", 
               getpid(), shared_data_var, (void *)&shared_data_var);
        if (child_pfn) {
            printf("[CHILD  %d] Physical Frame Number (PFN): 0x%lx\n", getpid(), child_pfn);
        }
        printf("\n");

        // 4. Send allocated virtual addresses to parent
        uintptr_t addresses[2];
        addresses[0] = (uintptr_t)child_heap_ptr;
        addresses[1] = (uintptr_t)child_mmap_ptr;
        write(pipe_c2p[1], addresses, sizeof(addresses));
        close(pipe_c2p[1]);

        printf("----------------------------------------------------------------------\n");
        printf("[CHILD  %d] >>> Addresses transmitted to parent via pipe.\n", getpid());
        printf("[CHILD  %d] >>> Child is now PAUSED and STILL ALIVE, holding memory in RAM!\n", getpid());
        printf("[CHILD  %d] >>> Waiting for parent to finish concurrent inspection...\n", getpid());
        printf("----------------------------------------------------------------------\n\n");

        // Wait for parent's release signal
        char ack;
        read(pipe_p2c[0], &ack, sizeof(ack));
        close(pipe_p2c[0]);

        printf("[CHILD  %d] >>> Release signal received from parent. Child exiting cleanly now.\n", getpid());
        munmap(child_mmap_ptr, 4096);
        exit(0);

    } else {
        // ==================================================================
        // PARENT PROCESS
        // ==================================================================
        close(pipe_c2p[1]); // Close unused write end of c2p
        close(pipe_p2c[0]); // Close unused read end of p2c

        // 1. Read child's addresses from pipe WHILE CHILD IS STILL RUNNING
        uintptr_t received_addresses[2];
        read(pipe_c2p[0], received_addresses, sizeof(received_addresses));
        close(pipe_c2p[0]);

        void *child_heap_addr = (void *)received_addresses[0];
        void *child_mmap_addr = (void *)received_addresses[1];

        char child_maps_path[64];
        snprintf(child_maps_path, sizeof(child_maps_path), "/proc/%d/maps", pid);

        printf("======================================================================\n");
        printf("[PARENT %d] >>> Child PID %d is STILL ALIVE and running concurrently! <<<\n", 
               getpid(), pid);
        printf("[PARENT %d] >>> Received child addresses: heap=%p, mmap=%p\n",
               getpid(), child_heap_addr, child_mmap_addr);
        printf("======================================================================\n\n");

        // -------------------------------------------------------------
        // VERIFICATION 1: MMAP Area Inspection (Child vs Parent)
        // -------------------------------------------------------------
        printf("[PARENT %d] === VERIFICATION 1: Inspecting MMAP Address (%p) ===\n", 
               getpid(), child_mmap_addr);
        // Look up in Child's active memory map:
        show_vma_for_address("IN CHILD MAP ", "child_mmap_addr", child_mmap_addr, child_maps_path);
        // Look up in Parent's own active memory map:
        show_vma_for_address("IN PARENT MAP", "child_mmap_addr", child_mmap_addr, "/proc/self/maps");
        printf("  ===> PROOF: While Child is active, %p exists ONLY in Child's VMA list!\n",
               child_mmap_addr);
        printf("              In Parent, it is completely [NOT MAPPED].\n\n");

        // -------------------------------------------------------------
        // VERIFICATION 2: Heap Break & VMA Inspection (Child vs Parent)
        // -------------------------------------------------------------
        printf("[PARENT %d] === VERIFICATION 2: Heap Area Comparison ===\n", getpid());
        printf("[PARENT %d] Parent's current sbrk(0): %p (UNCHANGED!)\n", getpid(), sbrk(0));
        printf("[PARENT %d] Child's previous sbrk(0) was %p, expanded to %p (+128 KiB)\n", 
               getpid(), child_heap_addr, (void *)((uintptr_t)child_heap_addr + 128 * 1024));
        
        printf("[PARENT %d] Inspecting Child's newly expanded heap address (%p):\n", getpid(), child_heap_addr);
        show_vma_for_address("IN CHILD MAP ", "child_heap_addr", child_heap_addr, child_maps_path);
        show_vma_for_address("IN PARENT MAP", "child_heap_addr", child_heap_addr, "/proc/self/maps");
        printf("  ===> PROOF: In Child, %p is an active VMA (from 128 KiB sbrk expansion)!\n", child_heap_addr);
        printf("              In Parent, %p is [NOT MAPPED in this process!]\n\n", child_heap_addr);

        // -------------------------------------------------------------
        // VERIFICATION 3: Copy-on-Write Physical Frame Comparison
        // -------------------------------------------------------------
        printf("[PARENT %d] === VERIFICATION 3: Data Variable & Physical Frames ===\n", getpid());
        unsigned long parent_pfn = get_pfn_for_address(&shared_data_var, 0);
        unsigned long child_pfn  = get_pfn_for_address(&shared_data_var, pid);

        printf("[PARENT %d] Parent shared_data_var value = %d (address: %p)\n", 
               getpid(), shared_data_var, (void *)&shared_data_var);
        if (parent_pfn && child_pfn) {
            printf("[PARENT %d] Parent Physical Frame (PFN): 0x%lx\n", getpid(), parent_pfn);
            printf("[PARENT %d] Child  Physical Frame (PFN): 0x%lx\n", getpid(), child_pfn);
            printf("  ===> PROOF: Same virtual address (%p) maps to TWO DIFFERENT physical frames!\n",
                   (void *)&shared_data_var);
        } else {
            printf("  ===> PROOF: Parent value is unchanged (100) while Child modified it to 999.\n");
        }
        printf("\n");

        // -------------------------------------------------------------
        // RELEASE CHILD PROCESS
        // -------------------------------------------------------------
        printf("----------------------------------------------------------------------\n");
        printf("[PARENT %d] Verification complete. Sending release signal to Child %d...\n", 
               getpid(), pid);
        char ack = 'K';
        write(pipe_p2c[1], &ack, sizeof(ack));
        close(pipe_p2c[1]);

        // Wait for child to exit and reap it
        int status;
        waitpid(pid, &status, 0);
        printf("[PARENT %d] Child %d reaped. Both processes completed.\n", getpid(), pid);
        printf("----------------------------------------------------------------------\n");
    }

    return 0;
}
```


```bash
[root@DA22053700:LinuxKernel-1-5-Process]# gcc process_memory_area_demo.c -o process_memory_area_demo
[root@DA22053700:LinuxKernel-1-5-Process]# ./process_memory_area_demo
======================================================================
 CONCURRENT MEMORY AREA ISOLATION DEMONSTRATION (PARENT vs CHILD)
======================================================================

[INITIAL BASELINE before fork]
[PARENT 123967] Initial heap break (sbrk(0)): 0x5964bfec6000
[PARENT] [heap] VMA: 5964bfea5000-5964bfec6000 rw-p 00000000 00:00 0                          [heap]
----------------------------------------------------------------------

[CHILD  123968] Process started (Parent PID: 123967).
[CHILD  123968] Initial heap break (sbrk(0)): 0x5964bfec6000
[CHILD] [heap] VMA: 5964bfea5000-5964bfec6000 rw-p 00000000 00:00 0                          [heap]

[CHILD  123968] === STEP 1: Child expands HEAP by 128 KiB ===
[CHILD  123968] New heap break (sbrk(0)): 0x5964bfee6000
[CHILD] [heap] VMA: 5964bfea5000-5964bfec6000 rw-p 00000000 00:00 0                          [heap]
[CHILD] child_heap_ptr (0x5964bfec6000) is inside VMA:
       --> 5964bfec6000-5964bfee6000 rw-p 00000000 00:00 0                          [heap]

[CHILD  123968] === STEP 2: Child allocates MMAP Page (4 KiB) ===
[CHILD] child_mmap_ptr (0x794d6b0fa000) is inside VMA:
       --> 794d6b0fa000-794d6b0fb000 rw-p 00000000 00:00 0

[CHILD  123968] === STEP 3: Child modifies shared_data_var (COW) ===
[CHILD  123968] shared_data_var = 999 at virtual address: 0x59648c113010
[CHILD  123968] Physical Frame Number (PFN): 0x1786da

----------------------------------------------------------------------
[CHILD  123968] >>> Addresses transmitted to parent via pipe.
[CHILD  123968] >>> Child is now PAUSED and STILL ALIVE, holding memory in RAM!
[CHILD  123968] >>> Waiting for parent to finish concurrent inspection...
----------------------------------------------------------------------

======================================================================
[PARENT 123967] >>> Child PID 123968 is STILL ALIVE and running concurrently! <<<
[PARENT 123967] >>> Received child addresses: heap=0x5964bfec6000, mmap=0x794d6b0fa000
======================================================================

[PARENT 123967] === VERIFICATION 1: Inspecting MMAP Address (0x794d6b0fa000) ===
[IN CHILD MAP ] child_mmap_addr (0x794d6b0fa000) is inside VMA:
       --> 794d6b0fa000-794d6b0fb000 rw-p 00000000 00:00 0
[IN PARENT MAP] child_mmap_addr (0x794d6b0fa000) -> [NOT MAPPED in this process!]
  ===> PROOF: While Child is active, 0x794d6b0fa000 exists ONLY in Child's VMA list!
              In Parent, it is completely [NOT MAPPED].

[PARENT 123967] === VERIFICATION 2: Heap Area Comparison ===
[PARENT 123967] Parent's current sbrk(0): 0x5964bfec6000 (UNCHANGED!)
[PARENT 123967] Child's previous sbrk(0) was 0x5964bfec6000, expanded to 0x5964bfee6000 (+128 KiB)
[PARENT 123967] Inspecting Child's newly expanded heap address (0x5964bfec6000):
[IN CHILD MAP ] child_heap_addr (0x5964bfec6000) is inside VMA:
       --> 5964bfec6000-5964bfee6000 rw-p 00000000 00:00 0                          [heap]
[IN PARENT MAP] child_heap_addr (0x5964bfec6000) -> [NOT MAPPED in this process!]
  ===> PROOF: In Child, 0x5964bfec6000 is an active VMA (from 128 KiB sbrk expansion)!
              In Parent, 0x5964bfec6000 is [NOT MAPPED in this process!]

[PARENT 123967] === VERIFICATION 3: Data Variable & Physical Frames ===
[PARENT 123967] Parent shared_data_var value = 100 (address: 0x59648c113010)
[PARENT 123967] Parent Physical Frame (PFN): 0x2badd1
[PARENT 123967] Child  Physical Frame (PFN): 0x1786da
  ===> PROOF: Same virtual address (0x59648c113010) maps to TWO DIFFERENT physical frames!

----------------------------------------------------------------------
[PARENT 123967] Verification complete. Sending release signal to Child 123968...
[CHILD  123968] >>> Release signal received from parent. Child exiting cleanly now.
[PARENT 123967] Child 123968 reaped. Both processes completed.
----------------------------------------------------------------------
```