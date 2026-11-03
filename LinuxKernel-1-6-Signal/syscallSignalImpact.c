#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

void handle_signal(int sig) {
    char msg[256];
    int len;
    
    if (sig == SIGUSR1) {
        len = snprintf(msg, sizeof(msg), "\n  -> [Signal Handler] Caught SIGUSR1 (No SA_RESTART flag).\n");
    } else if (sig == SIGUSR2) {
        len = snprintf(msg, sizeof(msg), "\n  -> [Signal Handler] Caught SIGUSR2 (With SA_RESTART flag).\n");
    }
    write(STDOUT_FILENO, msg, len);
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    printf("Starting System Call / Signal Interaction Demonstration...\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // ---------------------------------------------------------
        // CHILD PROCESS
        // ---------------------------------------------------------
        close(pipefd[1]); // Close unused write end
        
        char buffer[64];
        ssize_t bytes_read;

        // 1. Setup SIGUSR1 WITHOUT SA_RESTART
        struct sigaction sa1;
        sa1.sa_handler = handle_signal;
        sigemptyset(&sa1.sa_mask);
        sa1.sa_flags = 0; // No restart flag!
        sigaction(SIGUSR1, &sa1, NULL);

        // 2. Setup SIGUSR2 WITH SA_RESTART
        struct sigaction sa2;
        sa2.sa_handler = handle_signal;
        sigemptyset(&sa2.sa_mask);
        sa2.sa_flags = SA_RESTART; // Tells the kernel to restart interrupted system calls
        sigaction(SIGUSR2, &sa2, NULL);

        printf("[Child] Test 1: Blocking on read() - waiting for SIGUSR1...\n");
        
        // This read() will block indefinitely because the pipe is empty
        bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        
        if (bytes_read == -1) {
            if (errno == EINTR) {
                printf("[Child] RESULT 1: read() failed with EINTR (Interrupted system call)!\n");
                printf("                  The signal handler ran, but the system call was aborted.\n\n");
            } else {
                perror("[Child] read failed unexpectedly");
            }
        }

        printf("[Child] Test 2: Blocking on read() again - waiting for SIGUSR2...\n");
        
        // Block again. This time we wait for SIGUSR2 (which has SA_RESTART)
        bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("[Child] RESULT 2: read() COMPLETED successfully and read: '%s'\n", buffer);
            printf("                  The signal handler ran, and the kernel automatically RESTARTED read()!\n");
        } else if (bytes_read == -1 && errno == EINTR) {
            printf("[Child] RESULT 2: read() failed with EINTR (This shouldn't happen with SA_RESTART!).\n");
        }

        close(pipefd[0]);
        printf("\n[Child] Exiting cleanly.\n");
        exit(0);

    } else {
        // ---------------------------------------------------------
        // PARENT PROCESS
        // ---------------------------------------------------------
        close(pipefd[0]); // Close unused read end
        
        sleep(2); // Let child block on first read()
        
        printf("[Parent] Sending SIGUSR1 to child (configured WITHOUT SA_RESTART)...\n");
        kill(pid, SIGUSR1);

        sleep(3); // Let child process the failure and block on second read()
        
        printf("[Parent] Sending SIGUSR2 to child (configured WITH SA_RESTART)...\n");
        kill(pid, SIGUSR2);

        sleep(2); // Wait a bit. The child is STILL blocked in read()!
        
        printf("[Parent] Writing data to the pipe so the restarted read() can finish...\n");
        write(pipefd[1], "Hello from Parent!", 18);

        close(pipefd[1]);
        wait(NULL);
        printf("[Parent] Child finished. Exiting.\n");
    }

    return 0;
}
