// Необходимо реализовать программу, которая будет читать содержимое большого 
// текстового файла в несколько потоков. Каждый поток должен прочитать свою 
// часть файла, а затем передать её для обработки или анализа.

#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>

class MultithreadReader {
    std::string file_name;
    std::streamsize file_size;

    std::vector<char> buffer;

    std::vector<std::thread> tasks;
public:
    MultithreadReader(const std::string& file_name) 
        : file_name(file_name) {

        std::ifstream file(file_name, std::ios::binary);

        if (file.is_open()) {
            file.seekg(0, std::ios::end);
            file_size = file.tellg();
            file.close(); 

            buffer.resize(file_size);
        }
        else {
            file_size = 0;
        }

    }

    ~MultithreadReader() {
        joinAll();
    }

    void read(unsigned int thread_count) {
        int size = file_size / thread_count;

        for (int i = 0; i < thread_count; ++i)
            tasks.emplace_back(&MultithreadReader::readWithOffset, this, file_name, size, i * size);

        joinAll();
    }

    char* getBuffer() {
        return buffer.data();
    }

    long long getBufferSize() {
        return buffer.size();
    }

private:
    void readWithOffset(const std::string& file_name, long long size, long long offset) {
        std::ifstream file(file_name, std::ios::binary);
        if (file.is_open()) {
            file.seekg(offset);

            file.read(buffer.data() + offset, size);
        }
        file.close();
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
