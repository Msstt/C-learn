#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;
using namespace std::placeholders;

#define BUFFER_SIZE 1024

io_service service;

class Client : public std::enable_shared_from_this<Client> {
public:
  static std::shared_ptr<Client> Start(ip::tcp::endpoint endpoint,
                                       std::string data) {
    std::shared_ptr<Client> client(new Client(data));
    client->Start(endpoint);
    return client;
  }

  void Stop() {
    this->started_ = false;
    this->socket_.close();
  }

  bool IsStarted() { return this->started_; }

private:
  Client(std::string data) : socket_(service), data_(data) {}

  void Start(ip::tcp::endpoint endpoint) {
    this->socket_.async_connect(
        endpoint, std::bind(&Client::OnConnect, shared_from_this(), _1));
  }

  void DoWrite() {
    if (!this->started_) {
      return;
    }
    data_ += "\n";
    this->socket_.async_write_some(
        buffer(data_), std::bind(&Client::OnWrite, shared_from_this(), _1, _2));
  }

  void DoRead() {
    async_read(this->socket_, buffer(this->buffer_, BUFFER_SIZE),
               std::bind(&Client::ReadComplete, shared_from_this(), _1, _2),
               std::bind(&Client::OnRead, shared_from_this(), _1, _2));
  }

  void OnConnect(const error_code &err) { this->DoWrite(); }

  void OnWrite(const error_code &err, size_t bytes) { this->DoRead(); }

  void OnRead(const error_code &err, size_t bytes) {
    std::string recv_data(this->buffer_, bytes - 1);
    this->data_.pop_back();
    std::cout << "recv: " << recv_data << " ,data is "
              << (this->data_ == recv_data ? "ok" : "err") << std::endl;
    this->Stop();
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
  std::string data_;
  bool started_{true};
  char buffer_[BUFFER_SIZE];
};

int main() {
  std::string data;
  std::cin >> data;
  Client::Start(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8080),
                data);
  service.run();
  return 0;
}