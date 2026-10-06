#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Linux Process Generation Demo
 *
 * Demonstrates:
 * 1. fork()   - Clones the calling process to create a child.
 *               Under Linux, glibc's fork() invokes the clone() / clone3() system call
 *               without CLONE_VM, duplicating the task_struct, file descriptors,
 *               and setting up Copy-On-Write (COW) for memory pages.
 * 2. execlp() - Front-end for the execve() system call.
 *               Replaces the current process image (text, data, heap, stack)
 *               with the new executable ("ls"), while keeping the same PID.
 * 3. waitpid()- Front-end for the wait4() system call.
 *               Suspends parent execution until the child state changes (exits),
 *               preventing the child from remaining a "zombie" process.
 */

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
        // If execlp succeeds, it never returns.
        execlp("ls", "ls", "-lahtr", NULL);

        // If we reach this line, execlp failed
        perror("execlp failed");
        return 1;
    } else {
        // Parent branch: fork() returns the child's PID to the parent process
        printf("[PARENT] Created child PID %d.\n", pid);
        
        int status;
        // Wait for child to finish and collect its exit status
        pid_t reaped_pid = waitpid(-1, &status, 0);
        
        printf("\n[PARENT] Child %d has exited. (Raw status: %d)\n", reaped_pid, status);
        if (WIFEXITED(status)) {
            printf("[PARENT] Child exited normally with code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[PARENT] Child terminated by signal: %d\n", WTERMSIG(status));
        }
        printf("[PARENT] Done.\n");
    }
    return 0;
}
