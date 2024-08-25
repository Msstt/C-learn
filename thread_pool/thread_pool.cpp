#include "thread_pool.h"

ThreadPool::ThreadPool(size_t thread_num) {
    this->init(thread_num);
}

ThreadPool::~ThreadPool() {
    this->stop();
}

auto ThreadPool::init(size_t thread_num) -> bool {
    if (!this->threads_.empty()) {
        return false;
    }
    this->threads_.resize(thread_num);
    return true;
}

void ThreadPool::start() {
    if (this->threads_.empty() || this->threads_.front() != nullptr || this->is_runinng_.load()) {
        return;
    }
    this->is_runinng_.store(true);
    for (size_t i = 0; i < this->threads_.size(); i++) {
        this->threads_[i] = new std::thread(&ThreadPool::run, this);
    }
}

void ThreadPool::stop() {
    if (!this->is_runinng_.load()) {
        return;
    }
    this->is_runinng_.store(false);
    this->task_cond_.notify_all();
    for (size_t i = 0; i < this->threads_.size(); i++) {
        this->threads_[i]->join();
        delete this->threads_[i];
        this->threads_[i] = nullptr;
    }
}

auto ThreadPool::isRunning() -> bool {
    return this->is_runinng_.load();
}

auto ThreadPool::getThreadNum() -> size_t {
    return this->threads_.size();
}

auto ThreadPool::getTaskNum() -> size_t {
    std::unique_lock<std::mutex> lock(this->tasks_mutex_);
    return this->tasks_.size();
}

auto ThreadPool::getTask() -> std::unique_ptr<Task> {
    std::unique_lock<std::mutex> lock(this->tasks_mutex_);
    this->task_cond_.wait(lock, [this]() {
        return !this->tasks_.empty() || !this->is_runinng_.load();
    });
    if (!this->is_runinng_.load()) {
        return nullptr;
    }
    auto temp_task = std::move(this->tasks_.front());
    this->tasks_.pop();
    return temp_task;
}

void ThreadPool::run() {
    while (this->is_runinng_.load()) {
        auto task = this->getTask();
        if (task != nullptr) {
            this->working_thread_num_.fetch_add(1);
            try {
                task->func_();
            } catch(...) {
                std::cerr << "task has error!" << std::endl;
            }
            this->working_thread_num_.fetch_add(-1);
            if (this->working_thread_num_.load() == 0) {
                this->empty_cond_.notify_all();
            }
        }
    }
}

auto ThreadPool::waitAllDone(size_t expire_time) -> bool {
    std::unique_lock<std::mutex> lock(this->tasks_mutex_);
    auto is_done = [this]() {
        return this->tasks_.empty() && this->working_thread_num_.load() == 0;
    };
    if (expire_time) {
        this->empty_cond_.wait_for(lock, std::chrono::milliseconds(expire_time), [this, is_done] {
            return is_done();
        });
        return is_done();
    } else {
        this->empty_cond_.wait(lock, [this, is_done] {
            return is_done();
        });
        return true;
    }
}