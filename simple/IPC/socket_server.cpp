#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

bool running = true;

void handler(int num) { running = 0; }

int main() {
  signal(SIGINT, handler);
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  int address_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  bind(server_fd, reinterpret_cast<sockaddr *>(&address), address_len);
  listen(server_fd, 5);
  char data[1024];
  while (running) {
    std::cout << "Waiting..." << std::endl;
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&address),
                           reinterpret_cast<socklen_t *>(&address_len));
    std::cout << "receive from " << inet_ntoa(address.sin_addr) << std::endl;
    read(client_fd, data, 1024);
    std::cout << data << std::endl;
    close(client_fd);
  }
  close(server_fd);
  return 0;
}
