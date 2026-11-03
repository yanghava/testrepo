#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handle_signal(int sig) {
    switch (sig) {
        case SIGINT:
            write(STDOUT_FILENO, "\nCaptured SIGINT (Ctrl+C)\n", 26);
            break;
        case SIGQUIT:
            write(STDOUT_FILENO, "\nCaptured SIGQUIT (Ctrl+\\)\n", 28);
            break;
        case SIGTSTP:
            write(STDOUT_FILENO, "\nCaptured SIGTSTP (Ctrl+Z). Exiting...\n", 39);
            exit(0);
        default:
            break;
    }
}

int main() {
    printf("PID: %d (Ctrl+C: SIGINT, Ctrl+\\: SIGQUIT, Ctrl+Z: Exit)\n", getpid());

    // Configure sigaction struct
    // sa_handler sets the signal disposition:
    // - Custom callback function (e.g., handle_signal)
    // - SIG_IGN (ignore signal)
    // - SIG_DFL (restore default action)
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Register signal handlers
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);

    while (1) {
        sleep(1);
    }

    return 0;
}
