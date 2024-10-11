#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  int fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
  ftruncate(fd, 1024);
  char *shm = static_cast<char *>(mmap(0, 1024, PROT_READ, MAP_SHARED, fd, 0));
  std::cout << shm << std::endl;
  munmap(shm, 1024);
  close(fd);
  shm_unlink("/my_shm");
  return 0;
}
