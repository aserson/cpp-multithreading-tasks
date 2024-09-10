#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <cstring>
#include <functional>
#include <filesystem>

#include "task-05.h"

void CreateTextFile(const std::filesystem::path& file_name, const std::size_t file_size, const char constant_letter = '\0') {
    if (std::filesystem::exists(file_name)) {
        std::cout << "Testfile " << file_name.string() << " exists" << std::endl;
        return;
    }

    std::cout << "Testfile " << file_name.string() << " generating... ";

    if (std::ofstream file(file_name, std::ios::out | std::ios::binary); file.is_open()) {
        if (constant_letter == '\0') {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(32, 126);

            for (std::size_t i = 0; i < file_size; ++i) {
                char letter = static_cast<char>(dist(gen));
                file.write(&letter, 1);
            }
        } else {
            for (std::size_t i = 0; i < file_size; ++i) {
                file.write(&constant_letter, 1);
            }
        }

        std::cout << "Done!" << std::endl;
        std::cout << "File generated: " << file_name << " (size: " << file_size << " bytes)" << std::endl;
    } else {
        std::cout << "Failed to open file." << std::endl;
    }
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

TEST(Taks05, EqualityTest) {
    CreateTextFile("testfile128mb.txt", 128 * 1024 * 1024);
    std::vector<char> computed;
    std::vector<char> expected;

    {
        MultiThreadReader reader_partially("testfile128mb.txt");
        reader_partially.read(4);
        const std::size_t computed_size = reader_partially.getBufferSize();

        computed.resize(computed_size);
        std::memcpy(computed.data(), reader_partially.getBuffer(), computed_size * sizeof(char));
    }

    {
        MultiThreadReader reader_fully("testfile128mb.txt");
        reader_fully.read(1);
        const std::size_t expected_size = reader_fully.getBufferSize();

        expected.resize(expected_size);
        std::memcpy(expected.data(), reader_fully.getBuffer(), expected_size * sizeof(char));
    }

    EXPECT_EQ(computed.size(), expected.size());

    for (long long i = 0; i < computed.size(); ++i) {
        ASSERT_EQ(computed[i], expected[i]) << "Mismatch at index " << i << " ("
                                            << static_cast<int>(computed[i]) << " vs "
                                            << static_cast<int>(expected[i]) << ")";
    }
}

TEST(Taks05, PerformanseTest) {
    CreateTextFile("testfile1024mb.txt", 1024 * 1024 * 1024);
    double n_threads_time;
    double one_thread_time;

    {
        MultiThreadReader reader_fully("testfile1024mb.txt");
        one_thread_time = Timer::measure(&MultiThreadReader::read, &reader_fully, 1);
    }

    {
        MultiThreadReader reader_partially("testfile1024mb.txt");
        n_threads_time = Timer::measure(&MultiThreadReader::read, &reader_partially, 2);
    }

    EXPECT_TRUE(n_threads_time < one_thread_time);

    std::cout << "One thread time: " << one_thread_time << std::endl;
    std::cout << "N thread time: " << n_threads_time << std::endl;
}

TEST(Taks05, EmptyFileTest) {
    CreateTextFile("testfile0b.txt", 0);

    MultiThreadReader reader("testfile0b.txt");

    reader.read(4);  // Use 4 threads
    EXPECT_EQ(reader.getBufferSize(), 0);  // Buffer size should be 0
}

TEST(Taks05, SmallFileWithManyThreadsTest) {
    CreateTextFile("testfile10bA.txt", 10, 'A');

    MultiThreadReader reader("testfile10bA.txt");

    reader.read(100);  // Use 100 threads, even though the file is only 10 bytes
    EXPECT_EQ(reader.getBufferSize(), 10);  // Buffer size should match file size

    const char* buffer = reader.getBuffer();
    for (std::size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(buffer[i], 'A');  // Check the contents of the file
    }
}

TEST(Taks05, InvalidFilePathTest) {
    EXPECT_THROW({
        MultiThreadReader reader("nonexistent_file.txt");
        reader.read(4);  // This should throw an exception
    }, std::runtime_error);
}

TEST(Taks05, FilePermissionTest) {
    EXPECT_THROW({
        MultiThreadReader reader("/root/restricted_file.txt");
        reader.read(4);
    }, std::runtime_error);
}
