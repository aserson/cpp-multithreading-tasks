// Необходимо реализовать программу, которая будет читать содержимое большого 
// текстового файла в несколько потоков. Каждый поток должен прочитать свою 
// часть файла, а затем передать её для обработки или анализа.

#pragma once

#include <fstream>
#include <thread>
#include <mutex>
#include <vector>

class MultiThreadReader {
    std::string file_name;
    std::streamsize file_size;

    std::vector<char> buffer;

    std::vector<std::thread> tasks;
    std::mutex mtx;

    std::exception_ptr exception_ptr = nullptr;
public:
    explicit MultiThreadReader(const std::string& file_name) : file_name(file_name), file_size(0) {
        open(file_name);
    }

    void open(const std::string& file_name) {
        this->file_name = file_name;
        if (std::ifstream file(file_name, std::ios::binary); file.is_open()) {
            file.seekg(0, std::ios::end);
            this->file_size = file.tellg();

            buffer.resize(this->file_size);
        } else {
            throw std::runtime_error("Could not open the file: " + file_name);
        }
    }

    ~MultiThreadReader() {
        joinAll();
    }

    void read(unsigned int const thread_count) {
        const std::size_t chunk_size = file_size / thread_count;

        for (int i = 0; i < thread_count; ++i) {
            const std::size_t offset = i * chunk_size;
            const std::size_t current_size = (i == thread_count - 1) ? (file_size - offset) : chunk_size;
            tasks.emplace_back(&MultiThreadReader::readWithOffset, this, current_size, offset);
        }

        joinAll();
    }

    [[nodiscard]] const char* getBuffer() const {
        return buffer.data();
    }

    [[nodiscard]] unsigned long getBufferSize() const {
        return buffer.size();
    }

private:
    void readWithOffset(const std::size_t current_size, const std::size_t offset) {
        try {
            if (std::ifstream file(file_name, std::ios::binary); file.is_open()) {
                file.seekg(static_cast<std::streamsize>(offset));

                file.read(buffer.data() + offset, static_cast<std::streamsize>(current_size));

                if (file.gcount() != static_cast<std::streamsize>(current_size)) {
                    throw std::runtime_error("Error reading file at offset " + std::to_string(offset));
                }
            } else {
                throw std::runtime_error("Error opening file in thread");
            }
        } catch (...) {
            std::lock_guard<std::mutex> lock(mtx);
            if (!exception_ptr) {
                exception_ptr = std::current_exception();
            }
        }
    }

    void joinAll() {
        for (auto& task : tasks) {
            if (task.joinable()) {
                task.join();
            }
        }
        tasks.clear();
    }
};
