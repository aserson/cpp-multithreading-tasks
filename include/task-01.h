#include <iostream>
#include <thread>
#include <mutex>
#include <deque>

std::mutex m;
std::deque<std::string> tasks;
const std::size_t max_tasks = 3;
std::condition_variable cond_var;

void add_task(std::string task) {
    std::unique_lock<std::mutex> lock(m);

    cond_var.wait(lock, [] { return tasks.size() < max_tasks; });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    tasks.push_back(task);
    std::cout << "We add " << task << std::endl;

    cond_var.notify_one();
}

void take_task() {
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

// int main() {
//     std::thread t1(add_task, "task 1");
//     std::thread t2(add_task, "task 2");
//     std::thread t3(add_task, "task 3");
//     std::thread t4(add_task, "task 4");
//     std::thread t5(take_task);
//     std::thread t6(take_task);
//     std::thread t7(take_task);
//     std::thread t8(take_task);
//     std::thread t9(take_task);

//     t1.join();
//     t2.join();
//     t3.join();
//     t4.join();
//     t5.join();
//     t6.join();
//     t7.join();
//     t8.join();
//     t9.join();

//     return 0;
// }
