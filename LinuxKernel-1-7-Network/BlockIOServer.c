#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Create a listening socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Allow immediate restart on port 8888 without waiting for TIME_WAIT
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind to 0.0.0.0:8888
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));

    // 3. Start listening
    listen(listen_fd, 5);
    printf("Server started on port 8888. Waiting for connections...\n");

    // 4. Loop to handle clients one by one (sequentially / blocking)
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        printf("\n[Server] Calling accept()...\n");
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        int client_port = ntohs(client_addr.sin_port);
        printf("[Server] Accepted client 127.0.0.1:%d (conn_fd=%d)! Waiting for its message...\n",
               client_port, conn_fd);

        // 5. Keep receiving messages from THIS client until it disconnects (recv returns 0)
        while (1) {
            char buf[128] = {0};
            ssize_t n = recv(conn_fd, buf, sizeof(buf) - 1, 0);
            if (n <= 0) {
                // n == 0 means the client called close(fd)
                break;
            }
            printf("[Server] Received from 127.0.0.1:%d -> %s\n", client_port, buf);

            // 6. Send a reply back to the client
            char reply[256];
            snprintf(reply, sizeof(reply), "Server received your message: '%s'", buf);
            send(conn_fd, reply, strlen(reply), 0);
        }

        // 7. Close this client's socket before looping back to accept() the next client
        close(conn_fd);
        printf("[Server] Client 127.0.0.1:%d disconnected (closed conn_fd=%d).\n",
               client_port, conn_fd);
    }

    close(listen_fd);
    return 0;
}
