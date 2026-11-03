#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

// We use the advanced signature for the signal handler to receive siginfo_t
// which is populated by the kernel in Phase 2.
void handle_signal(int sig, siginfo_t *info, void *ucontext) {
    char msg[512];
    int len;

    if (sig == SIGTERM) {
        len = snprintf(msg, sizeof(msg), "\n--- HANDLER START ---\n[Phase 3: User Space] Child caught SIGTERM. Exiting gracefully...\n--- HANDLER END ---\n");
        write(STDOUT_FILENO, msg, len);
        exit(0);
    } else {
        len = snprintf(msg, sizeof(msg), 
            "\n--- HANDLER START ---\n"
            "[Phase 3: User Space] Child caught signal %d (SIGUSR1).\n"
            "   -> The Kernel told us this was sent by PID: %d\n"
            "--- HANDLER END (Phase 4: Returning to main loop) ---\n\n", 
            sig, info->si_pid);
        write(STDOUT_FILENO, msg, len);
    }
}

int main() {
    printf("Starting Demonstration...\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // ---------------------------------------------------------
        // CHILD PROCESS: Sets up signal handling and runs a loop
        // ---------------------------------------------------------
        printf("Child  (PID %d): Starting up...\n", getpid());

        // Configure sigaction struct
        struct sigaction sa;
        
        // Use SA_SIGINFO to get the detailed siginfo_t struct from the kernel
        sa.sa_flags = SA_SIGINFO; 
        sa.sa_sigaction = handle_signal; 
        sigemptyset(&sa.sa_mask);

        // Register signal handlers
        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);

        int counter = 1;
        while (1) {
            printf("[Child Main Loop] Doing work... (Iteration %d)\n", counter++);
            // sleep() can be interrupted by signals. When the handler returns, 
            // the main loop resumes exactly at the next iteration.
            sleep(2); 
        }
    } else {
        // ---------------------------------------------------------
        // PARENT PROCESS: Sends signals to the child
        // ---------------------------------------------------------
        printf("Parent (PID %d): Spawned child (PID %d).\n", getpid(), pid);

        // Let the child run its loop a couple of times
        sleep(3); 

        printf("\n[Phase 1: Generation] Parent sending SIGUSR1 to child...\n");
        // kill() is the system call used to send signals to other processes
        kill(pid, SIGUSR1); 

        // Let the child resume its loop after handling the signal
        sleep(4); 

        printf("\n[Phase 1: Generation] Parent sending SIGTERM to child...\n");
        kill(pid, SIGTERM);

        // Wait for the child process to exit cleanly to prevent zombies
        wait(NULL); 
        printf("Parent (PID %d): Child finished. Exiting.\n", getpid());
    }

    return 0;
}
