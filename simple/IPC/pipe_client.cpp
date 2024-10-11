#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
  int fd = open("/tmp/my_pipe", O_WRONLY);
  char buf[1024];
  std::cin >> buf;
  write(fd, buf, strlen(buf) + 1);
  close(fd);
  return 0;
}
