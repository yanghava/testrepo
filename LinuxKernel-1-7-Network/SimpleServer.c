#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // 1. Create a listening socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Bind to 127.0.0.1:8888
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));

    // 3. Start listening
    listen(listen_fd, 5);
    printf("Server started. Waiting for connections...\n");

    // 4. Accept a client connection
    int conn_fd = accept(listen_fd, NULL, NULL);
    printf("Client connected!\n");

    // 5. Receive message from client
    char buf[128] = {0};
    recv(conn_fd, buf, sizeof(buf) - 1, 0);
    printf("Received from client: %s\n", buf);

    // 6. Send a reply back to the client
    char *reply = "Hello Client, message received!";
    send(conn_fd, reply, strlen(reply), 0);
    printf("Reply sent to client.\n");

    // 7. Close sockets
    close(conn_fd);
    close(listen_fd);
    return 0;
}