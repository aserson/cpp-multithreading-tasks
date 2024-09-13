// Пять философов сидят за круглым столом. Перед каждым философом лежит тарелка с едой, и между 
// каждым философом лежит одна вилка. Каждый философ чередует два состояния — ест или размышляет. 
// Чтобы поесть, философу нужны две вилки : одна лежит слева, а другая справа. После того как
// философ поел, он кладёт обе вилки обратно на стол и снова начинает размышлять.

#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

template <std::size_t PhilosophersCount>
class Table {
    std::mutex mtx;
    std::mutex forks[PhilosophersCount];
    std::vector<std::thread> philosophers;

    unsigned int time_for_thinking;
    unsigned int time_for_eating;

public:
    Table() : Table(2000, 2000) {}

    Table(const unsigned int time_for_thinking, const unsigned int time_for_eating)
        : time_for_thinking(time_for_thinking), time_for_eating(time_for_eating) { }

    ~Table() {
        for (auto &philosopher: philosophers) {
            if (philosopher.joinable())
                philosopher.join();
        }
    }

    void run(unsigned int eating_count) {
        for (int i = 0; i < PhilosophersCount; i++)
            philosophers.emplace_back(&Table::go_to_table, this, i, eating_count);
    }

private:
    static void processing(const int time) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

    void go_to_table(const int philosopher_id, const unsigned int eating_count) {
        int left_fork = philosopher_id;
        int right_fork = (philosopher_id + 1) % PhilosophersCount;

        for (int i = 0; i < eating_count; ++i) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                std::cout << "Philosopher " << philosopher_id << " is thinking." << std::endl;
            }
            processing(time_for_thinking);

            if (philosopher_id % 2) {
                std::unique_lock<std::mutex> lock_1(forks[left_fork]);
                std::unique_lock<std::mutex> lock_2(forks[right_fork]);
            }
            else {
                std::unique_lock<std::mutex> lock_1(forks[right_fork]);
                std::unique_lock<std::mutex> lock_2(forks[left_fork]);
            }

            {
                std::unique_lock<std::mutex> lock(mtx);
                std::cout << "Philosopher " << philosopher_id << " is eating." << std::endl;
            }
            processing(time_for_eating);

            {
                std::unique_lock<std::mutex> lock(mtx);
                std::cout << "Philosopher " << philosopher_id << " finished eating." << std::endl;
            }
        }
    }
};
