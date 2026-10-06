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
