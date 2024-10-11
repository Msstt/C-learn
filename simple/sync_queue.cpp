#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <chrono>

template<class T>
struct Queue {
public:
  Queue(size_t max_size):max_size_(max_size) {}
  bool empty() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->empty_();
  }
  size_t size() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->queue_.size();
  }
  void push(T item) {
    std::unique_lock<std::mutex> lock(this->mutex_);
    this->full_cond_.wait(lock, [this](){return !this->full_();});
    this->queue_.push(item);
    empty_cond_.notify_one();
  }
  T pop() {
    std::unique_lock<std::mutex> lock(this->mutex_);
    this->empty_cond_.wait(lock, [this](){return !this->empty_();});
    auto item = this->queue_.front();
    this->queue_.pop();
    full_cond_.notify_one();
    return item;
  }
private:
  bool empty_() {
    return this->queue_.size() == 0;
  }
  bool full_() {
    return this->queue_.size() == max_size_;
  }
  size_t max_size_;
  std::mutex mutex_{};
  std::condition_variable empty_cond_{};
  std::condition_variable full_cond_{};
  std::queue<T> queue_{};
};

Queue<int> queue_(10);

void fun1() {
  for (int i = 0; i < 20; i++) {
    queue_.push(i);
    std::cout << "put: " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void fun2() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  for (int i = 0; i < 20; i++) {
    auto item = queue_.pop();
    std::cout << "get: " << item << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

int main() {
  std::thread thread1(fun1);
  std::thread thread2(fun2);
  thread1.join();
  thread2.join();
  return 0;
}
