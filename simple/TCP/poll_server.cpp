#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/poll.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

int server_fd;

void handler(int num) {
  close(server_fd);
  exit(0);
}

int main() {
  signal(SIGINT, handler);
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  size_t address_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  bind(server_fd, reinterpret_cast<sockaddr *>(&address), address_len);
  listen(server_fd, 5);
  int client_cnt = 0;
  pollfd read_fds[MAX_CLIENTS + 1];
  read_fds[0].fd = server_fd;
  read_fds[0].events = POLLIN;
  for (size_t i = 1; i <= MAX_CLIENTS; i++) {
    read_fds[i].fd = -1;
    read_fds[i].events = POLLIN;
  }
  char buffer[BUFFER_SIZE];
  while (true) {
    if (poll(read_fds, MAX_CLIENTS + 1, -1) == -1) {
      std::cerr << "poll error" << std::endl;
      return -1;
    }
    if ((read_fds[0].revents & POLLIN) && client_cnt < MAX_CLIENTS) {
      int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&address),
                             reinterpret_cast<socklen_t *>(&address_len));
      std::cout << "Connect " << inet_ntoa(address.sin_addr) << " ..."
                << std::endl;
      write(client_fd, "Welcome ...", BUFFER_SIZE);
      for (size_t i = 1; i <= MAX_CLIENTS; i++) {
        if (read_fds[i].fd == -1) {
          client_cnt++;
          read_fds[i].fd = client_fd;
          break;
        }
      }
    }
    for (int i = 1; i <= MAX_CLIENTS; i++) {
      if (read_fds[i].revents & POLLIN) {
        int client_fd = read_fds[i].fd;
        if (read(client_fd, buffer, BUFFER_SIZE) == 0) {
          close(client_fd);
          client_cnt--;
          read_fds[i].fd = -1;
          continue;
        }
        std::cout << buffer << std::endl;
        std::cin >> buffer;
        write(client_fd, buffer, BUFFER_SIZE);
      }
    }
  }
}
