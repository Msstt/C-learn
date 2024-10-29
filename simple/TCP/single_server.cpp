#include <arpa/inet.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

int socket_fd;

void handler(int num) {
  close(socket_fd);
  exit(0);
}

int main() {
  signal(SIGINT, handler);
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  size_t address_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  bind(socket_fd, reinterpret_cast<sockaddr *>(&address), address_len);
  listen(socket_fd, 3);
  char buffer[BUFFER_SIZE];
  while (true) {
    int client_fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&address),
                           reinterpret_cast<socklen_t *>(&address_len));
    std::cout << "Connect " << inet_ntoa(address.sin_addr) << " ..."
              << std::endl;
    write(client_fd, "Welcome ...", BUFFER_SIZE);
    while (read(client_fd, buffer, BUFFER_SIZE) > 0) {
      std::cout << buffer << std::endl;
      std::cin >> buffer;
      write(client_fd, buffer, BUFFER_SIZE);
    }
    close(client_fd);
  }
}