#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <vector>
#include <algorithm> 
#include <condition_variable>

class CarWorkshop {
    std::mutex m;
    std::condition_variable cond_var;
    std::vector<bool> masters_status;
    std::vector<std::thread> cars;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dist;

public:
    explicit CarWorkshop(const std::size_t master_count) : gen(rd()), dist(1,4) {
        masters_status.resize(master_count, true);
    }

    ~CarWorkshop() {
        for (auto& car : cars) {
            if (car.joinable())
                car.join();
        }
    }

    void add_car(const std::string& car_name) {

        unsigned int time = dist(gen);
        cars.emplace_back(&CarWorkshop::go_to_repair, this, car_name, time);
    }

private:
    int find_free_master() {
        for (int i = 0; i < masters_status.size(); i++)
            if (masters_status[i])
                return i;

        return -1;
    }

    static void processing(const int time) {
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }

    bool is_free_master() {
        return std::any_of(masters_status.begin(), masters_status.end(), [](const bool status) { return status; });
    }

    void go_to_repair(const std::string &car_name, const int time) {
        std::unique_lock<std::mutex> lock(m);

        cond_var.wait(lock, [this] { return is_free_master(); });

        const int master_id = find_free_master();
        masters_status[master_id] = false;

        lock.unlock();
        processing(time);
        lock.lock();
          
        std::cout << "Car " << car_name << " was repaired by master " << master_id + 1 << " in " << time << " hours" << std::endl;
        masters_status[master_id] = true;
        cond_var.notify_all();
    }
};