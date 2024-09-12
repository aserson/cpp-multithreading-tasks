// Реализовать конкурентную FIFO очередь с ограничением на количество элементов. 
// Требования:
// - Очередь должна быть безопасной для использования в многопоточной среде. Это означает, что операции добавления(push) и удаления(pop) могут вызываться одновременно несколькими потоками, но выполнение этих операций должно быть корректно синхронизировано.
// - Очередь должна соблюдать принцип FIFO : элементы должны извлекаться в том порядке, в котором они были добавлены.
// - Очередь должна иметь лимит на количество элементов. Если очередь заполнена до лимита, то операция добавления нового элемента должна блокироваться до тех пор, пока не освободится место.
// - Если количество элементов в очереди достигло предела, то операция добавления нового элемента(push) должна блокироваться до тех пор, пока один из элементов не будет удалён.
// - Если в очереди нет элементов, то операция удаления элемента(pop) должна блокироваться до тех пор, пока не будет добавлен хотя бы один элемент.

#pragma once

#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

template <typename TQueue>
class ConcurrentFIFOQueue {
    std::mutex mtx;
    std::condition_variable push_var, pop_var;

    std::deque<TQueue> queue;
    std::size_t limit;
    std::size_t waiting_time;

public:
    ConcurrentFIFOQueue(std::size_t limit) : ConcurrentFIFOQueue(limit, 2000) {}
    ConcurrentFIFOQueue(std::size_t limit, std::size_t waiting_time) : limit(limit), waiting_time(waiting_time) {}

    void push(const TQueue& val) {
        std::unique_lock lock(mtx);

        if (!push_var.wait_for(lock, std::chrono::milliseconds(waiting_time), [this] () {
            return queue.size() < limit;
            })) {
            std::cout << "Waiting more then " << waiting_time << "ms." << std::endl;
            throw std::runtime_error("Timeout waiting for push");
        }

        queue.push_back(val);
        pop_var.notify_one();
    }

    TQueue pop() {
        std::unique_lock lock(mtx);

        if (!pop_var.wait_for(lock, std::chrono::milliseconds(waiting_time), [this] () {
            return !queue.empty();
            })) {
            std::cout << "Waiting more then " << waiting_time << "ms." << std::endl;
            throw std::runtime_error("Timeout waiting for pop");
        }

        TQueue v = queue.front();
        queue.pop_front();
        push_var.notify_one();

        return v;
    }
};
