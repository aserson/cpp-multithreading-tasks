// Реализуйте шаблон "Чтение-Запись" с приоритетом для читателей или писателей. Используйте
// std::mutex и std::condition_variable, чтобы несколько потоков могли одновременно читать
// данные, но только один поток мог записывать данные в любой момент времени.

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
    unsigned int prioritized_count = 0;
    unsigned int max_prioritized;
public:
    ReaderWriter() : ReaderWriter(Users::Writers) {}

    explicit ReaderWriter(const Users priority) : ReaderWriter(priority, 3) {}

    explicit ReaderWriter(const Users priority, const std::size_t max_prioritized)
        : priority(priority), max_prioritized(max_prioritized) {}

    ~ReaderWriter() {
        for (auto& thread : threads)
            if (thread.joinable())
                thread.join();
    }

    void add_writer(const std::string& input) {
        threads.emplace_back(&ReaderWriter::write_process, this, input);
    }

    void add_reader(const unsigned int position, const unsigned int length) {
        threads.emplace_back(&ReaderWriter::read_process, this, position, length);
    }

    void set_priority(const Users users) {
        priority = users;
        prioritized_count = 0;
    }

    void set_priority(const Users users, const unsigned int prioritized) {
        priority = users;
        max_prioritized = prioritized;
        prioritized_count = 0;
    }

private:
    static void process(const unsigned int time) {
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

    void read_process(const unsigned int position, const unsigned int length) {
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
            std::cout << "Buffer length: " << buffer.size() << std::endl;
        }

        active_readers--;
        if (active_readers == 0) {
            if (priority == Readers && prioritized_count >= max_prioritized)
                prioritized_count = 0;
            writer_cond.notify_one();
        }
    }
};
