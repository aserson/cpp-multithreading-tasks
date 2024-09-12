// Реализуйте механизм барьерной синхронизации. Это такой синхронизационный механизм, 
// где все потоки должны достичь определенной точки в своей работе, прежде чем они 
// смогут продолжить выполнение.

#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

enum Users {
    Writers,
    Readers,
    None
};

class ReaderWriter {
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::condition_variable reader_cond;
    std::condition_variable writer_cond;

    std::string buffer;
    bool writer_active = false;
    int active_readers = 0;
    int waiting_writers = 0;

    Users priority;
    std::size_t prioritized_count = 0;
    std::size_t max_prioritized;
public:
    ReaderWriter() : ReaderWriter(Users::Writers) {}

    ReaderWriter(Users priority) : ReaderWriter(priority, 3) {}

    ReaderWriter(Users priority, std::size_t max_prioritized)
        : priority(priority), buffer(""), max_prioritized(max_prioritized)
    {}

    ~ReaderWriter() {
        for (auto& thread : threads)
            if (thread.joinable())
                thread.join();
    }

    void add_writer(const std::string& input) {
        threads.emplace_back(&ReaderWriter::write_process, this, input);
    }

    void add_reader(const std::size_t position, const std::size_t length) {
        threads.emplace_back(&ReaderWriter::read_process, this, position, length);
    }

    void set_priority(Users users) {
        priority = users;
        prioritized_count = 0;
    }

    void set_priority(Users users, const std::size_t prioritized) {
        priority = users;
        max_prioritized = prioritized;
        prioritized_count = 0;
    }

private:
    void process(const std::size_t time) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

    void write_process(const std::string& input) {
        std::unique_lock<std::mutex> lock(mtx);

        waiting_writers++;

        writer_cond.wait(lock, [this]() {
            return !writer_active && active_readers == 0 &&
                (priority == Writers || prioritized_count == 0);
            });

        writer_active = true;
        waiting_writers--;
        if (priority == Writers)
            prioritized_count++;

        buffer += input;
        process(input.size());
        std::cout << "Writer wrote: " << input << std::endl;

        writer_active = false;

        if (priority == Writers && prioritized_count >= max_prioritized)
                prioritized_count = 0;

        if (priority == Readers && waiting_writers == 0) {
            reader_cond.notify_all();
        }
        else {
            writer_cond.notify_one();
        }
    }

    void read_process(const std::size_t position, const std::size_t length) {
        std::unique_lock<std::mutex> lock(mtx);

        reader_cond.wait_for(lock, std::chrono::seconds(2), [this, position, length]() {
            return !writer_active && !buffer.empty() && position + length <= buffer.size();
            });

        active_readers++;
        if (priority == Readers)
            prioritized_count++;

        process(length);
        if (position + length <= buffer.size()) {
            std::cout << "Reader read: " << std::string(buffer.begin() + position, buffer.begin() + position + length) << std::endl;
        } else {
            std::cout << "Reader can't read symbols " << position << " through " << position + length << std::endl;
            std::cout << "Buffer lenght: " << buffer.size() << std::endl;
        }

        active_readers--;
        if (active_readers == 0) {
            if (priority == Readers && prioritized_count >= max_prioritized)
                prioritized_count = 0;
            writer_cond.notify_one();
        }
    }
};
