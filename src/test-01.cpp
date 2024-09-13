#include "task-01.h"

int main() {
    std::thread t1(add_task, "task 1");
    std::thread t2(add_task, "task 2");
    std::thread t3(add_task, "task 3");
    std::thread t4(add_task, "task 4");
    std::thread t5(take_task);
    std::thread t6(take_task);
    std::thread t7(take_task);
    std::thread t8(take_task);
    std::thread t9(take_task);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

    return 0;
}
