#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

int server_fd;

void handler(int num) {
  close(server_fd);
  exit(0);
}

void setNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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
  int epoll_fd = epoll_create1(0);
  epoll_event event, events[MAX_EVENTS];
  event.events = EPOLLIN;
  event.data.fd = server_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
  char buffer[BUFFER_SIZE];
  while (true) {
    int event_cnt = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < event_cnt; i++) {
      if (events[i].data.fd == server_fd) {
        int client_fd =
            accept(server_fd, reinterpret_cast<sockaddr *>(&address),
                   reinterpret_cast<socklen_t *>(&address_len));
        std::cout << "Connect " << inet_ntoa(address.sin_addr) << " ..."
                  << std::endl;
        write(client_fd, "Welcome ...", BUFFER_SIZE);
        setNonBlock(client_fd);
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        event.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
      } else {
        int client_fd = events[i].data.fd;
        bool is_close = false;
        while (true) {
          int read_cnt = read(client_fd, buffer, BUFFER_SIZE);
          if (read_cnt == -1) {
            if (errno == EAGAIN) {
              break;
            }
            std::cout << "read has error" << std::endl;
            exit(0);
          } else if (read_cnt == 0) {
            close(client_fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            is_close = true;
            break;
          }
        }
        if (is_close) {
          continue;
        }
        std::cout << buffer << std::endl;
        std::cin >> buffer;
        write(client_fd, buffer, BUFFER_SIZE);
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        event.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &event);
      }
    }
  }
}
