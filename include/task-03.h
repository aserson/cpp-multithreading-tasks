#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <vector>
#include <algorithm>

class Washhouse {
    std::mutex m;
    std::condition_variable cond_var_1;
    std::condition_variable cond_var_2;
    std::vector<bool> washers;
    std::vector<bool> dryers;
    std::vector<std::thread> clothes;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dist1;
    std::uniform_int_distribution<> dist2;
public:
    Washhouse(const std::size_t& washers_count, const std::size_t& dryers_count) : gen(rd()), dist1(1,4), dist2(1, 3) {
        washers.resize(washers_count, true);
        dryers.resize(dryers_count, true);
    }

    ~Washhouse() {
        for (auto& c : clothes) {
            if (c.joinable())
                c.join();
        }
    }

    void add_clothes(const std::string& clothes_name) {
        unsigned int time_1 = dist1(gen);
        unsigned int time_2 = dist2(gen);
        clothes.emplace_back(&Washhouse::go_to_wash, this, clothes_name, time_1, time_2);
    }

private:
    static bool is_free_machine(const std::vector<bool>& machines) {
        return std::any_of(machines.begin(), machines.end(), [](const bool status) { return status; });
    }

    static int find_free_machine(const std::vector<bool>& machines) {
        for (int i = 0; i < machines.size(); i++)
            if (machines[i])
                return i;
        return -1;
    }

    static void processing(const int time) {
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }

    void go_to_wash(const std::string& clothes_name, const int time_1, const int time_2) {
        std::unique_lock<std::mutex> lock(m);

        cond_var_1.wait(lock, [this] { return is_free_machine(washers); });
        int machine_id = find_free_machine(washers);
        washers[machine_id] = false;

        lock.unlock();
        processing(time_1);
        lock.lock();

        std::cout << clothes_name << " Clothes was washed by washer " << machine_id + 1 << " in " << time_1 << " hours" << std::endl;
        washers[machine_id] = true;
        cond_var_1.notify_all();

        cond_var_2.wait(lock, [this] { return is_free_machine(dryers); });
        machine_id = find_free_machine(dryers);
        dryers[machine_id] = false;

        lock.unlock();
        processing(time_2);
        lock.lock();

        std::cout << clothes_name << " Clothes was dried by dryer " << machine_id + 1 << " in " << time_2 << " hours" << std::endl;
        dryers[machine_id] = true;
        cond_var_2.notify_all();
    }
};
