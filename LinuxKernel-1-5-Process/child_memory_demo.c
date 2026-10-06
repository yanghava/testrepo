#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdint.h>

/*
 * Child Process Memory Acquisition & Address Space Isolation Demo
 *
 * Key Concepts Demonstrated:
 * 1. Virtual Address Space Isolation:
 *    Parent and child possess distinct 'struct mm_struct' descriptors in the kernel.
 *    They have independent page tables (different CR3 register values on x86).
 *
 * 2. Copy-On-Write (COW):
 *    Variables initialized before fork() initially share physical frames marked read-only.
 *    When either process writes, a page fault triggers the kernel to duplicate the page.
 *
 * 3. Dynamic Memory Acquisition by Child:
 *    - Heap expansion via brk() / sbrk() / malloc()
 *    - Direct kernel page mapping via mmap() (MAP_PRIVATE | MAP_ANONYMOUS)
 *
 * 4. Cross-Process Address Accessibility:
 *    Memory acquired by the child is NOT accessible to the parent via pointer sharing,
 *    because pointers are purely *virtual* addresses interpreted in the caller's context.
 */

int global_var = 100; // Lives in the Data segment

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return 1;
    }

    // Allocate a shared page using MAP_SHARED for comparison
    int *shared_page = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *shared_page = 1;

    printf("============================================================\n");
    printf("[PARENT init] PID: %d\n", getpid());
    printf("[PARENT init] global_var address : %p (value: %d)\n", (void *)&global_var, global_var);
    printf("[PARENT init] shared_page address: %p (value: %d)\n", (void *)shared_page, *shared_page);
    printf("[PARENT init] Initial heap break (sbrk(0)): %p\n", sbrk(0));
    printf("============================================================\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // -------------------------------------------------------------
        // CHILD PROCESS
        // -------------------------------------------------------------
        close(pipefd[0]); // Close unused read end of pipe

        printf("[CHILD] PID: %d, PPID: %d\n", getpid(), getppid());
        printf("[CHILD] Heap break before allocation: %p\n", sbrk(0));

        // 1. Trigger Copy-On-Write on existing variable
        global_var = 999;
        printf("[CHILD] Modified global_var to %d at address: %p\n", global_var, (void *)&global_var);

        // 2. Child acquires memory from system via malloc() (Heap / brk)
        char *child_heap = (char *)malloc(64);
        strcpy(child_heap, "Child private data in HEAP");
        printf("[CHILD] Acquired 64 bytes via malloc() at: %p\n", (void *)child_heap);
        printf("[CHILD] Heap break after malloc: %p\n", sbrk(0));

        // 3. Child acquires memory directly from kernel via mmap() (Memory Mapping Area)
        char *child_mmap = (char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        strcpy(child_mmap, "Child private data in MMAP page");
        printf("[CHILD] Acquired 4KB page via mmap() at: %p\n", (void *)child_mmap);

        // 4. Modify MAP_SHARED page
        *shared_page = 777;
        printf("[CHILD] Modified MAP_SHARED page to: %d\n", *shared_page);

        // Send child_heap pointer address (numerical value) to parent via pipe
        uintptr_t child_ptr_val = (uintptr_t)child_heap;
        write(pipefd[1], &child_ptr_val, sizeof(child_ptr_val));
        close(pipefd[1]);

        printf("[CHILD] Sent heap pointer address (0x%lx) to parent via pipe.\n\n", child_ptr_val);
        free(child_heap);
        munmap(child_mmap, 4096);
        exit(0);
    } else {
        // -------------------------------------------------------------
        // PARENT PROCESS
        // -------------------------------------------------------------
        close(pipefd[1]); // Close unused write end of pipe

        uintptr_t received_ptr = 0;
        read(pipefd[0], &received_ptr, sizeof(received_ptr));
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        printf("------------------------------------------------------------\n");
        printf("[PARENT resume after child exits]\n");
        printf("[PARENT] global_var address : %p (value: %d)\n", (void *)&global_var, global_var);
        printf("         -> Note: Parent's global_var is UNCHANGED (COW protected it)!\n\n");

        printf("[PARENT] shared_page value  : %d\n", *shared_page);
        printf("         -> Note: MAP_SHARED reflects child's modification (777).\n\n");

        printf("[PARENT] Received child's heap pointer address: 0x%lx\n", received_ptr);
        printf("         -> Can parent dereference this pointer (0x%lx)?\n", received_ptr);
        printf("         -> NO! In parent's address space, 0x%lx is either unmapped\n", received_ptr);
        printf("            or points to completely different parent memory.\n");
        printf("         -> Process address spaces are strictly isolated by hardware MMU / page tables.\n");
        printf("------------------------------------------------------------\n");

        munmap(shared_page, sizeof(int));
    }

    return 0;
}
