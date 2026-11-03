#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <string.h>

long long start_time_ms = 0;

long long get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void handle_signal(int sig, siginfo_t *info, void *ucontext) {
    long long now_ms = get_time_ms() - start_time_ms;
    int sent_time = info ? info->si_value.sival_int : 0;
    
    char msg[512];
    int len = snprintf(msg, sizeof(msg), 
        "\n    >>> [%lld ms] [HANDLER START] Signal caught!\n"
        "    >>>           -> Signal sent by parent at: [%d ms]\n"
        "    >>>           -> Latency: %d ms\n"
        "    >>> [%lld ms] [HANDLER END]\n\n", 
        now_ms, sent_time, (int)now_ms - sent_time, get_time_ms() - start_time_ms);
    
    write(STDOUT_FILENO, msg, len);
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) { perror("pipe"); exit(1); }

    start_time_ms = get_time_ms();
    printf("[%lld ms] Starting Demonstration (Signal before System Call)...\n\n", get_time_ms() - start_time_ms);

    pid_t pid = fork();

    if (pid < 0) { perror("Fork failed"); exit(1); }

    if (pid == 0) {
        // ---------------------------------------------------------
        // CHILD PROCESS
        // ---------------------------------------------------------
        close(pipefd[1]); 
        
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO; 
        sa.sa_sigaction = handle_signal; 
        sigemptyset(&sa.sa_mask);
        sigaction(SIGUSR1, &sa, NULL);

        // Sleep for 2 seconds in user-space to guarantee the parent sends 
        // the signal BEFORE we attempt the pipe read()
        long long now = get_time_ms() - start_time_ms;
        printf("[%lld ms] [Child] Delaying my slow system call by 2 seconds...\n", now);
        
        // Note: sleep() is also an interruptible system call. But wait!
        // We will see what happens to this sleep() call when the signal hits.
        sleep(2); 

        now = get_time_ms() - start_time_ms;
        printf("[%lld ms] [Child] Now attempting SLOW system call: read() from empty pipe.\n", now);
        
        char buffer[64];
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        
        now = get_time_ms() - start_time_ms;
        
        if (bytes_read == -1 && errno == EINTR) {
            printf("[%lld ms] [Child] read() RETURNED -1 with EINTR.\n", now);
        } else {
            printf("[%lld ms] [Child] read() returned %zd.\n", now, bytes_read);
        }

        close(pipefd[0]);
        exit(0);

    } else {
        // ---------------------------------------------------------
        // PARENT PROCESS
        // ---------------------------------------------------------
        close(pipefd[0]); 
        
        // Sleep for 1 second. The child is sleeping for 2 seconds.
        // Therefore, we send the signal exactly 1 second BEFORE the child 
        // attempts its pipe read() system call.
        sleep(1); 
        
        long long send_time = get_time_ms() - start_time_ms;
        printf("[%lld ms] [Parent] Sending SIGUSR1 to child (Child has NOT called read() yet)...\n", send_time);
        
        union sigval sv;
        sv.sival_int = (int)send_time;
        sigqueue(pid, SIGUSR1, sv);

        // Wait a few seconds, then kill the child so the program doesn't hang forever
        // (since the child's read() will block indefinitely)
        sleep(3);
        printf("\n[%lld ms] [Parent] Sending SIGKILL to terminate the child.\n", get_time_ms() - start_time_ms);
        kill(pid, SIGKILL);

        wait(NULL); 
        printf("[%lld ms] [Parent] Exiting.\n", get_time_ms() - start_time_ms);
    }

    return 0;
}
