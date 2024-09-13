#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

inline std::mutex m;
inline std::deque<std::string> tasks;
constexpr std::size_t max_tasks = 3;
inline std::condition_variable cond_var;

inline void add_task(const std::string &task) {
    std::unique_lock<std::mutex> lock(m);

    cond_var.wait(lock, [] { return tasks.size() < max_tasks; });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    tasks.push_back(task);
    std::cout << "We add " << task << std::endl;

    cond_var.notify_one();
}

inline void take_task() {
    std::unique_lock<std::mutex> lock(m);

    if (cond_var.wait_for(lock, std::chrono::seconds(5), [] { return !tasks.empty(); })) {

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "We take " << tasks[0] << std::endl;
        tasks.pop_front();

        cond_var.notify_one();
    }
    else {
        std::cout << "We have no tasks " << std::endl;
    }
}