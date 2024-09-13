// Реализуйте механизм барьерной синхронизации. Это такой синхронизационный механизм, 
// где все потоки должны достичь определенной точки в своей работе, прежде чем они 
// смогут продолжить выполнение.

#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>

class Barrier {
    std::size_t num_threads;
    std::size_t current;
    bool flag;

    std::mutex mtx;
    std::condition_variable var;
public:
    explicit Barrier(const std::size_t num_threads)
        : num_threads(num_threads), current(0), flag(false) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);

        current++;
        if (current == num_threads) {
            flag = !flag;
            current = 0;
            var.notify_all();
        }
        else {
            bool local_flag = flag;
            var.wait(lock, [this, local_flag]() { return local_flag != flag; });
        }
    }
};
