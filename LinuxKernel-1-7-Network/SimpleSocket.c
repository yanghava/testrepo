#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("fd: %d\n", fd);
    while(1);
    return 0;
}