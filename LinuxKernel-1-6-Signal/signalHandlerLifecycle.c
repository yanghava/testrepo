#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t keep_running = 1;
long long start_time_ms = 0;

// Helper to get monotonic time in milliseconds
long long get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void handle_signal(int sig, siginfo_t *info, void *ucontext) {
    char msg[512];
    int len;
    long long now_ms = get_time_ms() - start_time_ms;

    if (sig == SIGTERM || sig == SIGINT) {
        len = snprintf(msg, sizeof(msg), 
            "\n--- HANDLER START ---\n"
            "[%lld ms] [Phase 3: User Space] Child caught signal %d. Shutting down...\n"
            "--- HANDLER END ---\n", now_ms, sig);
        write(STDOUT_FILENO, msg, len);
        
        keep_running = 0; 
    } else {
        // Extract the payload sent by the parent via sigqueue()
        int sent_time = info ? info->si_value.sival_int : 0;
        int latency = (int)now_ms - sent_time;
        
        len = snprintf(msg, sizeof(msg), 
            "\n--- HANDLER START ---\n"
            "[%lld ms] [Phase 3: User Space] Child caught signal %d.\n"
            "   -> Sent by PID: %d at %d ms (Latency: %d ms)\n"
            "--- HANDLER END (Phase 4: Returning to calculation) ---\n\n", 
            now_ms, sig, info ? info->si_pid : -1, sent_time, latency);
        write(STDOUT_FILENO, msg, len);
    }
}

void heavy_calculation() {
    long long now = get_time_ms() - start_time_ms;
    printf("[%lld ms] [Child] Starting heavy calculation. CPU is busy...\n", now);
    unsigned long long counter = 0;
    
    while (keep_running) {
        counter++;
        /*if (counter % 300000000ULL == 0) {
            now = get_time_ms() - start_time_ms;
            printf("[%lld ms] [Child] Still calculating... (Iteration %llu)\n", now, counter);
        }
		*/
    }
    
    now = get_time_ms() - start_time_ms;
    printf("[%lld ms] [Child] Heavy calculation stopped gracefully, with counter=%llu.\n", now, counter);
}

int main() {
    start_time_ms = get_time_ms();
    printf("[%lld ms] Starting Interactive Signal Demonstration...\n\n", get_time_ms() - start_time_ms);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // ---------------------------------------------------------
        // CHILD PROCESS
        // ---------------------------------------------------------
        long long now = get_time_ms() - start_time_ms;
        printf("[%lld ms] Child  (PID %d): Starting up...\n", now, getpid());

        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO; 
        sa.sa_sigaction = handle_signal; 
        sigemptyset(&sa.sa_mask);

        sigaction(SIGUSR1, &sa, NULL); // Signal 10
        sigaction(SIGUSR2, &sa, NULL); // Signal 12
        sigaction(SIGTERM, &sa, NULL); // Signal 15
        sigaction(SIGINT,  &sa, NULL); // Signal 2

        // Block SIGUSR1 for a few seconds to show delay in processing
        sigset_t block_mask, old_mask;
        sigemptyset(&block_mask);
        sigaddset(&block_mask, SIGUSR1);
        
        now = get_time_ms() - start_time_ms;
        printf("\n[%lld ms] [Child] Briefly blocking SIGUSR1 (10) for 15 seconds during init...\n", now);
        sigprocmask(SIG_BLOCK, &block_mask, &old_mask);
        
        sleep(15);
		
		       
        now = get_time_ms() - start_time_ms;
        printf("[%lld ms] [Child] Init continues. Unblocking SIGUSR1.\n\n", now);
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
		
		
		int count =0;
		do {
			now = get_time_ms() - start_time_ms;
			printf ("\n[%lld ms] [Child] Continuosly being initialied with SIGUSR1 (10) releaed for processing during init...\n", now);
			sleep (5);
			
		} while (count++<4);

        heavy_calculation();
        
        now = get_time_ms() - start_time_ms;
        printf("[%lld ms] Child  (PID %d): Exiting.\n", now, getpid());
        exit(0);

    } else {
        // ---------------------------------------------------------
        // PARENT PROCESS
        // ---------------------------------------------------------
        long long now = get_time_ms() - start_time_ms;
        printf("[%lld ms] Parent (PID %d): Spawned child (PID %d).\n", now, getpid(), pid);
        
        sleep(1); 
        
        int sig_to_send;
        char input[64];

        while (1) {
            now = get_time_ms() - start_time_ms;
            printf("\n[%lld ms] [Parent] Enter a signal number to send (e.g., 10 for SIGUSR1, 15 for TERM, 9 for KILL, 0 to quit): ", now);
            
            if (fgets(input, sizeof(input), stdin) == NULL) {
                now = get_time_ms() - start_time_ms;
                printf("\n[%lld ms] [Parent] Input stream closed (EOF). Sending SIGTERM (15) to clean up...\n", now);
                kill(pid, SIGTERM);
                break;
            }

            sig_to_send = atoi(input);

            if (sig_to_send == 0) {
                now = get_time_ms() - start_time_ms;
                printf("[%lld ms] [Parent] Quitting. Sending SIGTERM (15) to child to clean up...\n", now);
                kill(pid, SIGTERM);
                break;
            }

            long long send_time = get_time_ms() - start_time_ms;
            printf("[%lld ms] [Parent] Sending signal %d to child (PID %d)...\n", send_time, sig_to_send, pid);
            
            // Construct the payload to send WITH the signal
            union sigval sv;
            sv.sival_int = (int)send_time;

            // Use sigqueue() instead of kill() to send the signal + payload
            if (sigqueue(pid, sig_to_send, sv) == -1) {
                perror("[Parent] sigqueue() failed (Did the child already die?)");
                break;
            }
            
            if (sig_to_send == SIGKILL || sig_to_send == SIGTERM || sig_to_send == SIGINT) {
                now = get_time_ms() - start_time_ms;
                printf("[%lld ms] [Parent] Sent a termination signal. Waiting for child...\n", now);
                break;
            }
            
            sleep(5); 
        }

        int status;
        wait(&status); 
        
        now = get_time_ms() - start_time_ms;
        printf("\n");
        if (WIFSIGNALED(status)) {
            printf("[%lld ms] Parent (PID %d): Child was TERMINATED abruptly by signal %d.\n", now, getpid(), WTERMSIG(status));
        } else if (WIFEXITED(status)) {
            printf("[%lld ms] Parent (PID %d): Child exited GRACEFULLY with code %d.\n", now, getpid(), WEXITSTATUS(status));
        }
        
        now = get_time_ms() - start_time_ms;
        printf("[%lld ms] Parent (PID %d): Exiting.\n", now, getpid());
    }

    return 0;
}
