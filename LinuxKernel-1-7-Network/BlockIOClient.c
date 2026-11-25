#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // 2. Configure server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 3. Connect to the server and verify connection succeeded
    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect to server failed");
        close(fd);
        return 1;
    }

    // Print our own local ephemeral port so we can match it on the server
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(local_addr);
    getsockname(fd, (struct sockaddr *)&local_addr, &len);
    printf("Connected to server! (My local client port is %d)\n", ntohs(local_addr.sin_port));

    // 4. Loop to continuously send user input until user types "close"
    while (1) {
        char msg[128] = {0};
        printf("\nType a message to send (or 'close' to disconnect): ");
        if (fgets(msg, sizeof(msg), stdin) == NULL) {
            break; // EOF (e.g. Ctrl+D)
        }
        msg[strcspn(msg, "\r\n")] = 0; // Remove trailing newline

        if (strlen(msg) == 0) {
            continue; // Ignore empty Enter key
        }

        // Exit loop and close socket if user explicitly types "close"
        if (strcmp(msg, "close") == 0) {
            printf("Closing connection to server...\n");
            break;
        }

        if (send(fd, msg, strlen(msg), 0) < 0) {
            perror("send failed");
            break;
        }
        printf("Sent to server: %s (waiting for reply...)\n", msg);

        // 5. Receive reply from server
        char buf[256] = {0};
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n < 0) {
            perror("recv failed");
            break;
        } else if (n == 0) {
            printf("Server closed the connection.\n");
            break;
        }
        printf("Received from server: %s\n", buf);
    }

    // 6. Close socket
    close(fd);
    printf("Socket closed.\n");
    return 0;
}
