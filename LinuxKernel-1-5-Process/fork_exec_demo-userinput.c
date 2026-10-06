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
