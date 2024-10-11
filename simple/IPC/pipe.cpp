#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

int main() {
  int fd[2];
  pipe(fd);
  if (fork() == 0) {
    close(fd[1]);
    char ch[1];
    read(fd[0], ch, 1);
    std::cout << ch[0] << std::endl;
    close(fd[0]);
  } else {
    close(fd[0]);
    char ch[1];
    std::cin >> ch[0];
    write(fd[1], ch, 1);
    close(fd[1]);
  }
  return 0;
}
