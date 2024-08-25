#include "gtest/gtest.h"
#include "thread_pool.h"

TEST(thread_pool, basic) {
    auto fun = [](size_t start_num, size_t end_num) -> size_t {
        int sum = 0;
        for (size_t i = start_num; i <= end_num; i++) {
            sum += i;
        }
        return sum;
    };
    ThreadPool thread_pool(8);
    thread_pool.start();
    auto ret1 = thread_pool.exec(fun, 1, 10);
    auto ret2 = thread_pool.exec(fun, 1, 100);
    ASSERT_EQ(ret1.get(), 55);
    ASSERT_EQ(ret2.get(), 5050);
}

TEST(thread_pool, refer) {
    auto fun = [](int& x, int y) {
        x = std::max(x, y);
    };
    ThreadPool thread_pool(8);
    thread_pool.start();
    int ret = 0;
    auto ret1 = thread_pool.exec(fun, std::ref(ret), 100);
    auto ret2 = thread_pool.exec(fun, std::ref(ret), 300);
    ret1.get();
    ret2.get();
    ASSERT_EQ(ret, 300);
}

TEST(thread_pool, super) {
    auto fun = [](size_t start_num, size_t end_num, size_t time) -> size_t {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        int sum = 0;
        for (size_t i = start_num; i <= end_num; i++) {
            sum += i;
        }
        return sum;
    };
    ThreadPool thread_pool(8);
    thread_pool.start();
    std::vector<std::future<size_t>> ret(20);
    for (int i = 0; i < 20; i++) {
        ret[i] = thread_pool.exec(fun, 1, i * 100, i * 10);
    }
    thread_pool.waitAllDone();
    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(ret[i].get(), (1 + i * 100) * i * 100 / 2);
    }
}

TEST(thread_pool, restart) {
    auto fun = [](size_t start_num, size_t end_num, size_t time) -> size_t {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        int sum = 0;
        for (size_t i = start_num; i <= end_num; i++) {
            sum += i;
        }
        return sum;
    };
    ThreadPool thread_pool(8);
    thread_pool.start();
    thread_pool.exec(fun, 1, 10, 1000);
    thread_pool.exec(fun, 1, 10, 1000);
    thread_pool.stop();
    auto ret1 = thread_pool.exec(fun, 1, 10, 1000);
    auto ret2 = thread_pool.exec(fun, 1, 10, 1000);
    thread_pool.start();
    ASSERT_EQ(ret1.get(), 55);
    ASSERT_EQ(ret2.get(), 55);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}