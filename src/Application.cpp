#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <random>
#include <vector>
#include <algorithm> 

class CarWorkshop {
    std::mutex m;
    std::condition_variable cond_var;
    std::vector<bool> masters_status;
    std::vector<std::thread> cars;

public:
    CarWorkshop(std::size_t master_count) {
        masters_status.resize(master_count, true);
    }

    ~CarWorkshop() {
        for (auto& car : cars) {
            if (car.joinable())
                car.join();
        }
    }

    void add_car(std::string car_name) {
        unsigned int time = 1 + rand() % 4;
        cars.emplace_back(&CarWorkshop::go_to_repair, this, car_name, time);
    }

private:
    int find_free_master() {
        for (int i = 0; i < masters_status.size(); i++)
            if (masters_status[i])
                return i;

        return -1;
    }

    void reparing(int time) {
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }

    bool is_free_master() {
        return std::any_of(masters_status.begin(), masters_status.end(), [](bool status) { return status; });
    }

    void go_to_repair(std::string car_name, int time) {
        std::unique_lock<std::mutex> lock(m);

        cond_var.wait(lock, [this] { return is_free_master(); });

        int master_id = find_free_master();
        masters_status[master_id] = false;

        lock.unlock();
        reparing(time);
        lock.lock();
          
        std::cout << "Car " << car_name << " was repaired by master " << master_id + 1 << " in " << time << " hours" << std::endl;
        masters_status[master_id] = true;
        cond_var.notify_all();
    }
};

int main() {
    CarWorkshop ws(3);

    ws.add_car("Red Granta Old");
    ws.add_car("Red Granta New");
    ws.add_car("Black Almera");
    ws.add_car("Yellow Kia");
    ws.add_car("Timoxa");

    return 0;
}
