#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

// Struct to pass client info safely to each worker thread
struct client_info {
    int conn_fd;
    int client_port;
};

// Worker thread function: handles ONE client independently
void *handle_client(void *arg) {
    struct client_info *info = (struct client_info *)arg;
    int conn_fd = info->conn_fd;
    int client_port = info->client_port;
    free(info);

    // Get Linux kernel Thread ID (TID) so you can match it in 'lsof' or 'ps -eLf'!
    pid_t tid = syscall(SYS_gettid);
    printf("  [Thread TID=%d] Handling client 127.0.0.1:%d on FD=%d (waiting for message...)\n",
           tid, client_port, conn_fd);

    // Blocks ONLY this worker thread until this client sends a message
    char buf[128] = {0};
    ssize_t n = recv(conn_fd, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        printf("  [Thread TID=%d] Received from 127.0.0.1:%d -> %s\n", tid, client_port, buf);

        char reply[256];
        snprintf(reply, sizeof(reply), "Handled by Thread TID=%d! Your msg: '%s'", tid, buf);
        send(conn_fd, reply, strlen(reply), 0);
    }

    close(conn_fd);
    printf("  [Thread TID=%d] Closed FD=%d for client 127.0.0.1:%d and exiting thread.\n",
           tid, conn_fd, client_port);
    return NULL;
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(listen_fd, 5);
    printf("[Main PID=%d] Multi-threaded Server listening on 8888 (listen_fd=%d)...\n",
           getpid(), listen_fd);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Main thread ONLY accepts connections, never blocks on recv()!
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

        struct client_info *info = malloc(sizeof(struct client_info));
        info->conn_fd = conn_fd;
        info->client_port = ntohs(client_addr.sin_port);

        // Create a detached worker thread for this client
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, info);
        pthread_detach(tid); // Automatically free thread resources when it finishes
    }

    close(listen_fd);
    return 0;
}
