#include "gtest/gtest.h"

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

#include "task-07.h"

std::pair<int, int> StringAnalysis(const std::string& output, std::size_t num_threads) {
    std::size_t before_barrier_count = 0;
    std::size_t after_barrier_count = 0;

    std::size_t all_thread_reached_barrier_count = 0;
    std::size_t all_thread_passed_barrier_count = 0;

    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        for (int i = 0; i < num_threads; i++) {
            std::string finding_line = "Thread " + std::to_string(i) + " work before barrier.";
            if (line.find(finding_line) != std::string::npos) {
                before_barrier_count++;
                break;
            }
        }

        if (before_barrier_count == num_threads) {
            before_barrier_count = 0;
            all_thread_reached_barrier_count++;
        }

        for (int i = 0; i < num_threads; i++) {
            std::string finding_line = "Thread " + std::to_string(i) + " passed barrier and continue continues to work.";
            if (line.find(finding_line) != std::string::npos) {
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


void Worker(int id, std::size_t num_runs, std::mutex* cout_mtx, Barrier& barrier) {
    for (int i = 0; i < num_runs; ++i) {
        {
            std::unique_lock<std::mutex> lock(*cout_mtx);
            std::cout << "Thread " << id << " work before barrier." << std::endl;;
        }

        barrier.wait();

        {
            std::unique_lock<std::mutex> lock(*cout_mtx);
            std::cout << "Thread " << id << " passed barrier and continue continues to work." << std::endl;;
        }
    }
}

void RunThreadsWithBarrierTest(std::size_t num_threads, std::size_t num_runs) {
    std::streambuf* original_buffer = std::cout.rdbuf();

    std::ostringstream captured_output;
    std::cout.rdbuf(captured_output.rdbuf());

    {
        Barrier barrier(num_threads);
        std::mutex cout_mtx;
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(Worker, i, num_runs, &cout_mtx, std::ref(barrier));
        }

        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout.rdbuf(original_buffer);

    std::vector<bool> status(num_threads, false);
    auto computed = StringAnalysis(captured_output.str(), num_threads);

    ASSERT_EQ(computed.first, num_runs);
    ASSERT_EQ(computed.second, num_runs);

}

TEST(Taks07, RunThreadsWithBarrierOneTimeTest) {
    RunThreadsWithBarrierTest(1, 1);
    RunThreadsWithBarrierTest(2, 1);
    RunThreadsWithBarrierTest(3, 1);
    RunThreadsWithBarrierTest(4, 1);
    RunThreadsWithBarrierTest(std::thread::hardware_concurrency(), 1);
}

TEST(Taks07, RunThreadsWithBarrierFewTimesTest) {
    RunThreadsWithBarrierTest(1, 5);
    RunThreadsWithBarrierTest(2, 5);
    RunThreadsWithBarrierTest(3, 5);
    RunThreadsWithBarrierTest(4, 5);
    RunThreadsWithBarrierTest(std::thread::hardware_concurrency(), 5);
}

TEST(Taks07, RunThreadsWithBarrierManyTimesTest) {
    RunThreadsWithBarrierTest(1, 100);
    RunThreadsWithBarrierTest(2, 100);
    RunThreadsWithBarrierTest(3, 100);
    RunThreadsWithBarrierTest(4, 100);
    RunThreadsWithBarrierTest(std::thread::hardware_concurrency(), 100);
}
