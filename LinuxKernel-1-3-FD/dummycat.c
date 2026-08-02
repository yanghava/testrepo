#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
  int fd = open(argv[1], O_RDONLY);
  char buffer[1024];
  ssize_t bytes_read;
  while ((bytes_read = read(fd, buffer, 1024)) > 0) {
    ssize_t bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
  }
  close(fd);
  return 0;
}
