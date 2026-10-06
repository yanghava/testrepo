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
 * Demonstration: How Parent is Notified of Child Death and Responds Accordingly
 *
 * 1. Notification Mechanism:
 *    When the child exits, the Linux kernel automatically sends a SIGCHLD signal
 *    to the parent task_struct.
 *
 * 2. Proving Notification Delivery:
 *    - The parent puts itself into a 10-second sleep (simulating background computation).
 *    - The child terminates after just 2 seconds with exit code 42.
 *    - The arrival of SIGCHLD immediately interrupts the parent's sleep() system call,
 *      waking the parent up ~8 seconds early!
 *
 * 3. Responding Accordingly:
 *    - The signal handler marks an atomic flag and captures the child's exit status.
 *    - The parent's main thread inspects the notification and executes a tailored response
 *      based on whether the child succeeded, failed, or completed a specific subtask.
 */

// Shared volatile flags set by signal handler
static volatile sig_atomic_t child_notified = 0;
static volatile sig_atomic_t reaped_child_pid = 0;
static volatile sig_atomic_t child_exit_code = 0;

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

// Asynchronous Kernel Notification Handler
void sigchld_handler(int sig) {
    (void)sig;
    int status;
    pid_t pid;

    // Reap child to prevent zombie and extract exit status
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        reaped_child_pid = pid;
        child_notified = 1;

        if (WIFEXITED(status)) {
            child_exit_code = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            child_exit_code = -WTERMSIG(status); // Negative indicates killed by signal
        }
    }
}

// Parent's tailored response logic based on child's outcome
void parent_respond_to_child(pid_t pid, int exit_code) {
    log_msg("[PARENT RESPONSE] ================================================\n");
    log_msg("[PARENT RESPONSE] Handling notification for finished Child PID: %d\n", pid);

    if (exit_code == 0) {
        log_msg("[PARENT RESPONSE] Action: Child succeeded normally (Code 0).\n");
        log_msg("[PARENT RESPONSE] -> Archiving temporary files and committing results.\n");
    } else if (exit_code == 42) {
        log_msg("[PARENT RESPONSE] Action: Child finished with SPECIAL CODE %d!\n", exit_code);
        log_msg("[PARENT RESPONSE] -> Triggering Phase 2 data processing workflow.\n");
    } else if (exit_code < 0) {
        log_msg("[PARENT RESPONSE] Action: Child crashed via signal %d!\n", -exit_code);
        log_msg("[PARENT RESPONSE] -> Launching diagnostic recovery routine.\n");
    } else {
        log_msg("[PARENT RESPONSE] Action: Child reported error code %d.\n", exit_code);
        log_msg("[PARENT RESPONSE] -> Retrying or logging error alert.\n");
    }
    log_msg("[PARENT RESPONSE] ================================================\n");
}

int main(void) {
    // 1. Set up SIGCHLD handler WITHOUT SA_RESTART
    // Omission of SA_RESTART ensures that blocking calls (like sleep) are
    // immediately interrupted when the signal arrives, proving instant notification!
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No SA_RESTART -> interrupts sleep() immediately
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction failed");
        return 1;
    }

    log_msg("[PARENT %d] Starting demonstration.\n", getpid());

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // --- CHILD PROCESS ---
        log_msg("[CHILD  %d] Child started. Working for 2 seconds...\n", getpid());
        sleep(2);

        log_msg("[CHILD  %d] Work done. Exiting with custom exit code 42!\n", getpid());
        exit(42); // Deliberate exit code to demonstrate parent inspecting it
    }

    // --- PARENT PROCESS ---
    log_msg("[PARENT %d] Created child PID %d.\n", getpid(), pid);
    log_msg("[PARENT %d] Parent begins a 10-second deep task (sleep(10))...\n", getpid());

    // sleep() returns the unslept seconds when interrupted by a signal
    unsigned int unslept = sleep(10);

    // 2. Proof of Notification Delivery:
    log_msg("\n[PARENT %d] >>> WOKE UP FROM SLEEP! <<<\n", getpid());
    log_msg("[PARENT %d] Sleep was interrupted with %u seconds remaining!\n", getpid(), unslept);
    log_msg("[PARENT %d] (This proves the kernel delivered SIGCHLD immediately when child died)\n\n", getpid());

    // 3. Responding Accordingly
    if (child_notified) {
        log_msg("[PARENT %d] Notification confirmed from kernel. Processing response...\n", getpid());
        parent_respond_to_child(reaped_child_pid, child_exit_code);
    } else {
        log_msg("[PARENT %d] No notification received (sleep completed naturally).\n", getpid());
    }

    log_msg("[PARENT %d] Demonstration complete. Parent exiting cleanly.\n", getpid());
    return 0;
}
