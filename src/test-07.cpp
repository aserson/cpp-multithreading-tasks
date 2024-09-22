#include "task-07.h"

#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <cassert>

std::pair<int, int> StringAnalysis(const std::string& output, const unsigned int num_threads) {
    unsigned int before_barrier_count = 0;
    unsigned int after_barrier_count = 0;

    unsigned int all_thread_reached_barrier_count = 0;
    unsigned int all_thread_passed_barrier_count = 0;

    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        for (int i = 0; i < num_threads; i++) {
            if (std::string finding_line = "Thread " + std::to_string(i) + " work before barrier."; line.find(finding_line) != std::string::npos) {
                before_barrier_count++;
                break;
            }
        }

        if (before_barrier_count == num_threads) {
            before_barrier_count = 0;
            all_thread_reached_barrier_count++;
        }

        for (int i = 0; i < num_threads; i++) {
            if (std::string finding_line = "Thread " + std::to_string(i) + " passed barrier and continue continues to work."; line.find(finding_line) != std::string::npos) {
                after_barrier_count++;
                break;
            }
        }

        if (after_barrier_count == num_threads) {
            after_barrier_count = 0;
            all_thread_passed_barrier_count++;
        }
    }

    if (before_barrier_count == 0 && after_barrier_count == 0)
        return { all_thread_reached_barrier_count, all_thread_passed_barrier_count };
    else
        return { -1, -1 };
}


void Worker(const int id, const unsigned int num_runs, std::mutex* out_mtx, Barrier& barrier) {
    for (int i = 0; i < num_runs; ++i) {
        {
            std::unique_lock<std::mutex> lock(*out_mtx);
            std::cout << "Thread " << id << " work before barrier." << std::endl;;
        }

        barrier.wait();

        {
            std::unique_lock<std::mutex> lock(*out_mtx);
            std::cout << "Thread " << id << " passed barrier and continue continues to work." << std::endl;;
        }
    }
}

void RunThreadsWithBarrierTest(const unsigned int num_threads, const unsigned int num_runs) {
    std::streambuf* original_buffer = std::cout.rdbuf();

    std::ostringstream captured_output;
    std::cout.rdbuf(captured_output.rdbuf());

    {
        Barrier barrier(num_threads);
        std::mutex out_mtx;
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(Worker, i, num_runs, &out_mtx, std::ref(barrier));
        }

        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout.rdbuf(original_buffer);

    std::vector<bool> status(num_threads, false);
    auto [fst, snd] = StringAnalysis(captured_output.str(), num_threads);

    assert(fst == num_runs);
    assert(snd == num_runs);
}

void RunThreadsWithBarrierOneTimeTest() {
    std::cout << "Test threads with barrier one time... ";
    RunThreadsWithBarrierTest(1, 1);
    RunThreadsWithBarrierTest(2, 1);
    RunThreadsWithBarrierTest(3, 1);
    RunThreadsWithBarrierTest(4, 1);
    RunThreadsWithBarrierTest(std::thread::hardware_concurrency(), 1);

    std::cout << "Done!" << std::endl;
}

void RunThreadsWithBarrierFewTimesTest() {
    std::cout << "Test threads with barrier few times... ";

    RunThreadsWithBarrierTest(1, 5);
    RunThreadsWithBarrierTest(2, 5);
    RunThreadsWithBarrierTest(3, 5);
    RunThreadsWithBarrierTest(4, 5);
    RunThreadsWithBarrierTest(std::thread::hardware_concurrency(), 5);

    std::cout << "Done!" << std::endl;
}

void RunThreadsWithBarrierManyTimesTest() {
    std::cout << "Test threads with barrier many times... ";

    RunThreadsWithBarrierTest(1, 100);
    RunThreadsWithBarrierTest(2, 100);
    RunThreadsWithBarrierTest(3, 100);
    RunThreadsWithBarrierTest(4, 100);
    RunThreadsWithBarrierTest(std::thread::hardware_concurrency(), 100);

    std::cout << "Done!" << std::endl;
}

int main() {
    RunThreadsWithBarrierOneTimeTest();
    RunThreadsWithBarrierFewTimesTest();
    RunThreadsWithBarrierManyTimesTest();

    return 0;
}