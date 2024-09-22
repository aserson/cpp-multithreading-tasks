// Необходимо реализовать программу, которая выполняет сортировку большого массива чисел, используя 
// многопоточность для повышения производительности. Массив должен быть разделён на несколько частей, 
// каждая из которых сортируется в отдельном потоке. После сортировки всех частей, необходимо 
// выполнить слияние (merge) отсортированных частей в один итоговый массив.

#pragma once

#include <thread>
#include <vector>
#include <algorithm>

class MultiThreadSorter {
    unsigned int thread_count;

    std::vector<std::thread> tasks;
public:
    explicit MultiThreadSorter(const unsigned int thread_count)
        : thread_count(thread_count) { }

    ~MultiThreadSorter() {
        joinAll();
    }

    std::vector<int> sort(std::vector<int> vec) {
        const unsigned int vec_size = vec.size();

        std::vector<int> output(vec_size);
        const unsigned int chunk_size = vec_size / thread_count;

        for (int i = 0; i < thread_count; ++i) {
            const unsigned int offset = i * chunk_size;
            const unsigned int current_size = (i == thread_count - 1) ? (vec_size - offset) : chunk_size;

            tasks.emplace_back(std::sort<std::vector<int>::iterator>, vec.begin() + offset, vec.begin() + offset + current_size);
        }

        joinAll();

        for (int i = 0; i < thread_count; ++i) {
            const unsigned int offset = i * chunk_size;
            const unsigned int current_size = (i == thread_count - 1) ? (vec_size - offset) : chunk_size;

            output = merge(output.begin(), output.begin() + offset, vec.begin() + offset, vec.begin() + offset + current_size);
        }

        return output;
    }

private:
    static std::vector<int> merge(std::vector<int>::iterator first1, std::vector<int>::iterator last1,
        std::vector<int>::iterator first2, std::vector<int>::iterator last2) {
        const std::size_t size1 = last1 - first1;
        const std::size_t size2 = last2 - first2;

        std::vector<int> output(size1 + size2);
        auto iter = output.begin();

        while (first1 != last1 || first2 != last2) {
            if (first2 == last2) {
                std::copy(first1, last1, iter);
                return output;
            }

            if (first1 == last1) {
                std::copy(first2, last2, iter);
                return output;
            }

            if (*first2 < *first1) {
                *iter = *first2;
                ++first2;
                ++iter;
            } else {
                *iter = *first1;
                ++first1;
                ++iter;
            }
        }

        return output;
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
