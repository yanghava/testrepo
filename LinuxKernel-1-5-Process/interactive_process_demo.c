#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/*
 * Concurrent Interactive Process Controller
 *
 * Demonstrates:
 * 1. IPC via UNIX Pipes: Parent receives user commands from terminal and dispatches
 *    work or termination instructions to the concurrent child process.
 * 2. Concurrent Execution: Parent and child can perform independent tasks simultaneously.
 * 3. Non-blocking Process Supervision: Parent monitors child health using waitpid(..., WNOHANG).
 * 4. User-Driven Continuation/Termination for BOTH processes.
 */

int main(void) {
    int parent_to_child[2]; // Pipe for parent -> child commands
    int child_to_parent[2]; // Pipe for child -> parent responses

    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe creation failed");
        return 1;
    }

    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (child_pid == 0) {
        // =========================================================
        // CHILD PROCESS (Worker loop driven by commands from pipe)
        // =========================================================
        close(parent_to_child[1]); // Close write end
        close(child_to_parent[0]); // Close read end

        char command;
        int child_task_count = 0;

        while (read(parent_to_child[0], &command, sizeof(command)) > 0) {
            if (command == 'c') {
                child_task_count++;
                printf("[CHILD %d] >>> Starting Child Task #%d...\n", getpid(), child_task_count);
                sleep(1); // Simulate child computation
                printf("[CHILD %d] >>> Completed Child Task #%d.\n", getpid(), child_task_count);

                char ack = 'K';
                write(child_to_parent[1], &ack, sizeof(ack));
            } else if (command == 'q') {
                printf("[CHILD %d] >>> Received termination command. Exiting cleanly.\n", getpid());
                break;
            }
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } else {
        // =========================================================
        // PARENT PROCESS (Interactive Control Console)
        // =========================================================
        close(parent_to_child[0]); // Close read end
        close(child_to_parent[1]); // Close write end

        int child_alive = 1;
        int parent_task_count = 0;
        char input[64];

        printf("============================================================\n");
        printf(" Concurrent Process Control Console (Parent PID: %d, Child PID: %d)\n", 
               getpid(), child_pid);
        printf("============================================================\n");

        while (1) {
            printf("\n--- Select an Action ---\n");
            printf(" [1] Continue Parent Task (Parent does work; Child runs in background)\n");
            if (child_alive) {
                printf(" [2] Continue Child Task  (Dispatch task to Child process)\n");
                printf(" [3] Check Child Status   (Non-blocking waitpid with WNOHANG)\n");
                printf(" [4] Terminate Child      (Send quit signal to Child)\n");
            } else {
                printf(" [Child is terminated]\n");
            }
            printf(" [5] Terminate Parent     (Exit program)\n");
            printf("Enter choice (1-5): ");
            fflush(stdout);

            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("\nExiting console.\n");
                break;
            }

            input[strcspn(input, "\n")] = '\0';
            int choice = atoi(input);

            if (choice == 1) {
                // Parent continues executing its own task
                parent_task_count++;
                printf("[PARENT %d] >>> Executing Parent Task #%d...\n", getpid(), parent_task_count);
                sleep(1); // Simulate parent work
                printf("[PARENT %d] >>> Parent Task #%d completed.\n", getpid(), parent_task_count);

            } else if (choice == 2 && child_alive) {
                // Command Child to continue working
                char cmd = 'c';
                write(parent_to_child[1], &cmd, sizeof(cmd));

                // Wait for Child acknowledgment
                char ack;
                read(child_to_parent[0], &ack, sizeof(ack));
                printf("[PARENT] Child finished its assigned task.\n");

            } else if (choice == 3 && child_alive) {
                // Non-blocking status check
                int status;
                pid_t res = waitpid(child_pid, &status, WNOHANG);
                if (res == 0) {
                    printf("[PARENT] Child (PID %d) is ALIVE and waiting for commands.\n", child_pid);
                } else if (res == child_pid) {
                    printf("[PARENT] Child (PID %d) has already terminated.\n", child_pid);
                    child_alive = 0;
                } else {
                    perror("waitpid");
                }

            } else if (choice == 4 && child_alive) {
                // Terminate Child
                char cmd = 'q';
                write(parent_to_child[1], &cmd, sizeof(cmd));
                
                int status;
                waitpid(child_pid, &status, 0); // Reap child
                printf("[PARENT] Child (PID %d) has been reaped.\n", child_pid);
                child_alive = 0;

            } else if (choice == 5) {
                // Terminate Parent
                printf("[PARENT] Terminating Parent process.\n");
                if (child_alive) {
                    // Clean up child before leaving
                    char cmd = 'q';
                    write(parent_to_child[1], &cmd, sizeof(cmd));
                    waitpid(child_pid, NULL, 0);
                }
                break;
            } else {
                printf("Invalid selection. Please choose 1-5.\n");
            }
        }

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        printf("[PARENT] Console closed. Done.\n");
    }

    return 0;
}
