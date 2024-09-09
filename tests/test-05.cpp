#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <cstring>
#include <functional>
#include <filesystem>

#include "task-05.h"

void CreateTextFile(const std::filesystem::path& file_name, unsigned int file_size_mb) {
    if (std::filesystem::exists(file_name)) {
        // std::cout << "Testfile " << file_name.string() << " exists" << std::endl;
        return;
    }

    std::cout << "Testfile " << file_name.string() << " generating... ";

    const std::size_t file_size = 1LL * file_size_mb * 1024 * 1024;
    std::ofstream file(file_name, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(32, 126);

    std::size_t generated_size = 0;
    const std::size_t chunk_size = 1024 * 1024;

    std::string buffer(chunk_size, '\0');

    while (generated_size < file_size) {
        for (std::size_t i = 0; i < chunk_size; ++i) {
            buffer[i] = static_cast<char>(dist(gen));
        }
        file.write(buffer.data(), chunk_size);
        generated_size += chunk_size;
    }

    file.close();
    std::cout << "Done!" << std::endl;
    std::cout << "File generated: " << file_name << " (size: " << file_size << " bytes)" << std::endl;
}

class Timer {
public:
    template<typename Func, typename... Args>
    static auto measure(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        return duration.count();
    }
};

TEST(Taks05, EqualityTest) {
    CreateTextFile("testfile128.txt", 128);
    std::vector<char> computed;
    std::vector<char> expected;

    {
        MultithreadReader reader_partially("../tests/testfile128.txt");
        reader_partially.read(4);
        int computed_size = reader_partially.getBufferSize();

        computed.resize(computed_size);
        std::memcpy(computed.data(), reader_partially.getBuffer(), computed_size * sizeof(char));
    }

    {
        MultithreadReader reader_fully("../tests/testfile128.txt");
        reader_fully.read(1);
        int expected_size = reader_fully.getBufferSize();

        expected.resize(expected_size);
        std::memcpy(expected.data(), reader_fully.getBuffer(), expected_size * sizeof(char));
    }

    EXPECT_EQ(computed.size(), expected.size());

    for (long long i = 0; i < computed.size(); ++i) {
        EXPECT_EQ(computed[i], expected[i]) << "Error at symblol " << i;
    }
}

TEST(Taks05, PerformanseTest) {
    CreateTextFile("testfile1024.txt", 1024);
    double n_threads_time;
    double one_thread_time;

    {
        MultithreadReader reader_fully("../tests/testfile1024.txt");
        one_thread_time = Timer::measure(&MultithreadReader::read, &reader_fully, 1);
    }

    {
        MultithreadReader reader_partially("../tests/testfile1024.txt");
        n_threads_time = Timer::measure(&MultithreadReader::read, &reader_partially, 4);
    }

    EXPECT_TRUE(n_threads_time < one_thread_time);

    std::cout << "One thread time: " << one_thread_time << std::endl;
    std::cout << "N thread time: " << n_threads_time << std::endl;
} 