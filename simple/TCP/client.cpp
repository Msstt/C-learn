#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#define BUFFER_SIZE 1024

int main() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(8080);
  if (connect(socket_fd, reinterpret_cast<sockaddr *>(&address),
              sizeof(address))) {
    std::cout << "Connect failed!" << std::endl;
    return -1;
  }
  char buffer[BUFFER_SIZE];
  read(socket_fd, buffer, BUFFER_SIZE);
  std::cout << buffer << std::endl;
  while (std::cin >> buffer) {
    write(socket_fd, buffer, BUFFER_SIZE);
    read(socket_fd, buffer, BUFFER_SIZE);
    std::cout << buffer << std::endl;
  }
  close(socket_fd);
  std::cout << "Connect close ..." << std::endl;
}
