#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>

long long start_time_ms = 0;

// Helper to get monotonic time in milliseconds
long long get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// Signal Handler
void handle_signal(int sig, siginfo_t *info, void *ucontext) {
    long long now_ms = get_time_ms() - start_time_ms;
    int sent_time = info ? info->si_value.sival_int : 0;
    int latency = (int)now_ms - sent_time;
    
    char msg[512];
    int len = snprintf(msg, sizeof(msg), 
        "\n    >>> [%lld ms] [HANDLER START] Child is executing the signal handler!\n"
        "    >>>           -> Signal sent by parent at: [%d ms]\n"
        "    >>>           -> Signal handled by child at: [%lld ms]\n"
        "    >>>           -> Latency: %d ms\n"
        "    >>>           The kernel aborted the slow read() system call to bring us here.\n"
        "    >>> [%lld ms] [HANDLER END]\n\n", 
        now_ms, sent_time, now_ms, latency, get_time_ms() - start_time_ms);
    
    write(STDOUT_FILENO, msg, len);
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    start_time_ms = get_time_ms();
    printf("[%lld ms] Starting Slow System Call Demonstration...\n\n", get_time_ms() - start_time_ms);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // ---------------------------------------------------------
        // CHILD PROCESS
        // ---------------------------------------------------------
        close(pipefd[1]); // Close unused write end of the pipe
        
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO; // Intentionally omitting SA_RESTART
        sa.sa_sigaction = handle_signal; 
        sigemptyset(&sa.sa_mask);
        sigaction(SIGUSR1, &sa, NULL);

        long long now = get_time_ms() - start_time_ms;
        printf("[%lld ms] [Child] About to invoke a SLOW system call: read() from an empty pipe.\n", now);
        printf("[%lld ms] [Child] I will go to sleep in Kernel Space indefinitely...\n", now);
        
        char buffer[64];
        
        // This is a SLOW system call. It will put the process to sleep.
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        
        now = get_time_ms() - start_time_ms;
        
        // Let's see what happened after the kernel woke us up!
        if (bytes_read == -1) {
            if (errno == EINTR) {
                printf("[%lld ms] [Child] read() RETURNED -1 with EINTR (Interrupted system call).\n", now);
                printf("[%lld ms] [Child] Notice how my main code resumes AFTER the handler finishes!\n", now);
            } else {
                perror("[Child] read failed unexpectedly");
            }
        }

        close(pipefd[0]);
        printf("[%lld ms] [Child] Exiting cleanly.\n", get_time_ms() - start_time_ms);
        exit(0);

    } else {
        // ---------------------------------------------------------
        // PARENT PROCESS
        // ---------------------------------------------------------
        close(pipefd[0]); // Close unused read end of the pipe
        
        // Let the child start up and get firmly blocked inside the read() system call
        sleep(2); 
        
        long long send_time = get_time_ms() - start_time_ms;
        printf("[%lld ms] [Parent] Child should be asleep in read() by now. Sending SIGUSR1...\n", send_time);
        
        // Construct the payload to send WITH the signal
        union sigval sv;
        sv.sival_int = (int)send_time;

        // sigqueue guarantees the child can read our exact send_time payload
        if (sigqueue(pid, SIGUSR1, sv) == -1) {
            perror("[Parent] sigqueue() failed");
        }

        // Wait for the child process to exit cleanly
        wait(NULL); 
        printf("\n[%lld ms] [Parent] Child finished. Exiting.\n", get_time_ms() - start_time_ms);
    }

    return 0;
}
