#include "gtest/gtest.h"

#include <iostream>
#include <random>
#include <chrono>
#include <cstring>
#include <functional>
#include <algorithm>

#include "task-06.h"

std::vector<int> CreateBuffer(const std::size_t buffer_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-100, 100);

    std::vector<int> output(buffer_size);
    std::generate(output.begin(), output.end(), std::bind(dist, std::ref(gen)));

    return output;
}

class Timer {
public:
    template<typename Func, typename... Args>
    static auto measure(Func&& func, Args&&... args) {
        const auto start = std::chrono::high_resolution_clock::now();
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> duration = end - start;

        return duration.count();
    }
};

void SortingTest(std::vector<int> input, const std::size_t threads_count) {
    std::cout << "Testing sorting results matching (buffer_length = " << input.size() << ", thread_count = " << threads_count << ")...";

    std::vector<int> output;

    {
        MultiThreadSorter sorter(threads_count);
        output = sorter.sort(input);
    }

    ASSERT_EQ(output.size(), input.size()) << "Sizes of input and output vectors are different";

    std::sort(input.begin(), input.end());

    for (int i = 0; i < output.size(); ++i)
        ASSERT_EQ(output[i], input[i]) << "Mismatch at index " << i << " ("
        << static_cast<int>(output[i]) << " vs "
        << static_cast<int>(input[i]) << ")";

    std::cout << " Done!" << std::endl;
}

TEST(Taks06, SmallArrayTest) {
    SortingTest(CreateBuffer(10), 1);
    SortingTest(CreateBuffer(10), 2);
    SortingTest(CreateBuffer(10), 3);
    SortingTest(CreateBuffer(10), 4);
    SortingTest(CreateBuffer(10), std::thread::hardware_concurrency());
}

TEST(Taks06, LargeArrayTest) {
    SortingTest(CreateBuffer(10000), 1);
    SortingTest(CreateBuffer(10000), 2);
    SortingTest(CreateBuffer(10000), 3);
    SortingTest(CreateBuffer(10000), 4);
    SortingTest(CreateBuffer(10000), std::thread::hardware_concurrency());
}

TEST(Taks06, AlreadySortedTest) {
    auto data = CreateBuffer(500);
    std::sort(data.begin(), data.end());

    SortingTest(data, 1);
    SortingTest(data, 2);
    SortingTest(data, 3);
    SortingTest(data, 4);
    SortingTest(data, std::thread::hardware_concurrency());
}

TEST(Taks06, ReverseSortedTest) {
    auto data = CreateBuffer(500);
    std::sort(data.begin(), data.end(), [](int lha, int rha) { return lha > rha; });

    SortingTest(data, 1);
    SortingTest(data, 2);
    SortingTest(data, 3);
    SortingTest(data, 4);
}

TEST(Taks06, EmptyArrayTest) {
    std::vector<int> data;

    SortingTest(data, 1);
    SortingTest(data, 2);
    SortingTest(data, 3);
    SortingTest(data, 4);
    SortingTest(data, std::thread::hardware_concurrency());
}
