#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
    // 1. Setup socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(listen_fd, 5);
    printf("Server started on 8888...\n");

    // 2. Setup epoll
    int epoll_fd = epoll_create1(0);
    struct epoll_event ev, events[10];

    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    // 3. Event Loop
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, 10, -1);

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listen_fd) {
                // New connection
                int conn_fd = accept(listen_fd, NULL, NULL);
                printf("Client connected!\n");

                ev.events = EPOLLIN;
                ev.data.fd = conn_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev);
            } else {
                // Client sent data or closed connection
                int conn_fd = events[i].data.fd;
                char buf[128] = {0};

                ssize_t n = recv(conn_fd, buf, sizeof(buf) - 1, 0);
                if (n <= 0) {
                    // n == 0 means client typed "close" and closed its socket
                    printf("Client (fd=%d) disconnected.\n", conn_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_fd, NULL);
                    close(conn_fd);
                } else {
                    printf("Received from fd=%d: %s\n", conn_fd, buf);

                    char reply[256];
                    snprintf(reply, sizeof(reply), "Hello Client (fd=%d), received via Epoll: '%s'", conn_fd, buf);
                    send(conn_fd, reply, strlen(reply), 0);
                    printf("Reply sent to fd=%d.\n", conn_fd);
                }
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}