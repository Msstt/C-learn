#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;
using namespace std::placeholders;

#define BUFFER_SIZE 1024

io_service service;

class Client : public std::enable_shared_from_this<Client> {
public:
  Client() : socket_(service) {}

  void Start() { this->DoRead(); }

  void Stop() {
    if (!this->started_) {
      return;
    }
    std::cout << "close socket" << std::endl;
    this->started_ = false;
    this->socket_.close();
  }

  ip::tcp::socket &socket() { return this->socket_; }

private:
  void DoWrite() {
    this->socket_.async_write_some(
        buffer(this->buffer_, this->bytes_),
        std::bind(&Client::OnWrite, shared_from_this(), _1, _2));
  }

  void DoRead() {
    async_read(this->socket_, buffer(this->buffer_, BUFFER_SIZE),
               std::bind(&Client::ReadComplete, shared_from_this(), _1, _2),
               std::bind(&Client::OnRead, shared_from_this(), _1, _2));
  }

  void OnWrite(const error_code &err, size_t bytes) { this->DoRead(); }

  void OnRead(const error_code &err, size_t bytes) {
    if (err) {
      Stop();
      return;
    }
    this->bytes_ = bytes;
    this->DoWrite();
  }

  size_t ReadComplete(const error_code &err, size_t bytes) {
    if (err) {
      return 0;
    }
    if (std::find(this->buffer_, this->buffer_ + bytes, '\n') <
        this->buffer_ + bytes) {
      return 0;
    } else {
      return 1;
    }
  }

  ip::tcp::socket socket_;
  bool started_{true};
  int bytes_;
  char buffer_[BUFFER_SIZE];
};

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8080));

void OnAccept(std::shared_ptr<Client> client, const error_code &err) {
  client->Start();
  std::shared_ptr<Client> new_client = std::make_shared<Client>();
  acceptor.async_accept(new_client->socket(),
                        std::bind(OnAccept, new_client, _1));
}

int main() {
  std::shared_ptr<Client> new_client = std::make_shared<Client>();
  acceptor.async_accept(new_client->socket(),
                        std::bind(OnAccept, new_client, _1));
  service.run();
  return 0;
}