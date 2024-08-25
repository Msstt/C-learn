/*
  @brief 非线程同步的线程池
  @date 2024/8/24
*/
#pragma once
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <future>
#include <queue>
#include <chrono>
#include <memory>
#include <iostream>

class ThreadPool {
public:
    ThreadPool() = default;
    ThreadPool(size_t thread_num);
    ~ThreadPool();

    /*
      @brief 初始化线程池
      @param thread_num: 线程数
      @return 是否成功
    */
    auto init(size_t thread_num) -> bool;

    /*
      @brief 获取线程池是否运行
      @return 是否运行
    */
    auto isRunning() -> bool;

    /*
      @brief 开启线程池
    */
    void start();

    /*
      @brief 停止线程池
    */
    void stop();

    /*
      @brief 获取线程数量
      @return 线程数量
    */
    auto getThreadNum() -> size_t;

    /*
      @brief 获取任务数量
      @return 任务数量
    */
    auto getTaskNum() -> size_t;

    /*
      @brief 添加任务
      @param expire_time: 超时时间, ms
             func: 任务函数
             args: 任务函数参数
      @return 任务结果
    */
    template<class F, class... Args>
    auto exec(F&& func, Args&&... args) -> std::future<decltype(func(args...))> {
        auto task = std::make_unique<Task>();
        using retType = decltype(func(args...));
        auto call_back = std::make_shared<std::packaged_task<retType(Args...)>>(func);
        task->func_ = [call_back, ...args_ = std::forward<Args>(args)]() mutable {
          (*call_back)(std::forward<Args>(args_)...);
        };
        // auto call_back = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        // task->func_ = [call_back]() {
        //   (*call_back)();
        // };
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        this->tasks_.push(std::move(task));
        this->task_cond_.notify_one();
        return call_back->get_future();
    }

    /*
      @brief 等待所有任务结束
      @param expire_time: 超时时间, ms, 不填为不超时
      @return 是否结束
    */
    auto waitAllDone(size_t expire_time = 0) -> bool;


private:
    struct Task {
        std::function<void()> func_;
    };

    auto getTask() -> std::unique_ptr<Task>;
    void run();

    std::vector<std::thread*> threads_{}; // 线程
    std::mutex tasks_mutex_{};
    std::queue<std::unique_ptr<Task>> tasks_{}; // 任务
    std::condition_variable task_cond_{};
    std::condition_variable empty_cond_{};
    std::atomic<size_t> working_thread_num_{}; // 当前工作线程数量
    std::atomic<bool> is_runinng_{};
};