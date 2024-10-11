#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool flg = true;

void handler(int num) { flg = false; }

int main() {
  signal(SIGINT, handler);
  const char *path = "/tmp/my_pipe";
  mkfifo(path, 0666);
  char buf[1024];
  while (flg) {
    int fd = open(path, O_RDONLY);
    read(fd, buf, sizeof(buf));
    std::cout << buf << std::endl;
    close(fd);
  }
  unlink(path);
  return 0;
}
