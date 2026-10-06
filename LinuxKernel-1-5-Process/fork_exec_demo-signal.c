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