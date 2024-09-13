#include "task-05.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <cstring>
#include <functional>
#include <filesystem>

void CreateTextFile(const std::filesystem::path& file_name, const std::size_t file_size, const char constant_letter = '\0') {
    if (exists(file_name)) {
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

void equality_test() {
    CreateTextFile("testfile128mb.txt", 128 * 1024 * 1024);

    std::cout << "Testing equality operations... ";
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

    assert(computed.size() == expected.size());

    for (long long i = 0; i < computed.size(); ++i) {
        if (computed[i] != expected[i]) {
            std::cout << "Mismatch at index "
                    << i << " (" << static_cast<int>(computed[i]) << " vs "
                                 << static_cast<int>(expected[i]) << ")";

            assert(false);
        }
    }

    std::cout << "Done!" << std::endl;
}

void performance_test() {
    CreateTextFile("testfile1024mb.txt", 1024 * 1024 * 1024);
    double n_threads_time;
    double one_thread_time;

    std::cout << "Performance test" << std::endl;
    {
        MultiThreadReader reader_fully("testfile1024mb.txt");
        one_thread_time = Timer::measure(&MultiThreadReader::read, &reader_fully, 1);
    }

    {
        MultiThreadReader reader_partially("testfile1024mb.txt");
        n_threads_time = Timer::measure(&MultiThreadReader::read, &reader_partially, 2);
    }

    assert(n_threads_time < one_thread_time);

    std::cout << "One thread time: " << one_thread_time << std::endl;
    std::cout << "N thread time: " << n_threads_time << std::endl;

    std::cout << "Done!" << std::endl;
}

void empty_file_test() {
    CreateTextFile("testfile0b.txt", 0);

    std::cout << "Empty file test... ";

    MultiThreadReader reader("testfile0b.txt");

    reader.read(4);  // Use 4 threads
    assert(reader.getBufferSize() == 0);  // Buffer size should be 0

    std::cout << "Done!" << std::endl;
}

void small_file_with_many_threads_test() {
    CreateTextFile("testfile10bA.txt", 10, 'A');

    std::cout << "Small file with many threads... ";

    MultiThreadReader reader("testfile10bA.txt");

    reader.read(100);  // Use 100 threads, even though the file is only 10 bytes
    assert(reader.getBufferSize() == 10);  // Buffer size should match file size

    const char* buffer = reader.getBuffer();
    for (std::size_t i = 0; i < 10; ++i) {
        assert(buffer[i] == 'A');  // Check the contents of the file
    }

    std::cout << "Done!" << std::endl;
}

void invalid_file_path_test() {
    std::cout << "Testing invalid file path... ";

    try {
        MultiThreadReader reader("nonexistent_file.txt");
        reader.read(4);
    } catch (std::runtime_error& error) {
        if (std::strcmp(error.what(), "Could not open the file: nonexistent_file.txt") == 0) {
            std::cout << "Done!" << std::endl;
            return;
        }
    }

    std::cout << "Failed!" << std::endl;
}

void file_permission_test() {
    std::cout << "File Permission Test... ";

    try {
        MultiThreadReader reader("/root/restricted_file.txt");
        reader.read(4);
    } catch (std::runtime_error& error) {
        if (std::strcmp(error.what(), "Could not open the file: /root/restricted_file.txt") == 0) {
            std::cout << "Done!" << std::endl;
            return;
        }
    }

    std::cout << "Failed!" << std::endl;
}

int main() {
    equality_test();
    performance_test();
    empty_file_test();
    small_file_with_many_threads_test();
    invalid_file_path_test();
    file_permission_test();

    return 0;
}