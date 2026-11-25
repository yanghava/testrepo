#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 3. Connect to the server
    connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Connected to server!\n");

    // 4. Send message to server
    char *msg = "Hello Server!";
    send(fd, msg, strlen(msg), 0);
    printf("Sent to server: %s\n", msg);

    // 5. Receive reply from server
    char buf[128] = {0};
    recv(fd, buf, sizeof(buf) - 1, 0);
    printf("Received from server: %s\n", buf);

    // 6. Close socket
    close(fd);
    return 0;
}