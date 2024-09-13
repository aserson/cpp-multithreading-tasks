// Напишите программу, которая выполняет параллельное умножение двух матриц с использованием нескольких потоков. 
// Каждый поток должен обрабатывать часть строк одной из матриц.

#pragma once

#include <iostream>
#include <vector>
#include <thread>

typedef std::vector<std::vector<int>> Matrix;

class MultiThreadMatrixMultiplicator {
    std::vector<std::thread> threads;
    const std::size_t num_threads;

    Matrix left, right;
    unsigned int rows_left, cols_left, rows_right, cols_right;
public:
    MultiThreadMatrixMultiplicator(const Matrix& left, const Matrix& right, const unsigned int num_threads) : num_threads(num_threads), left(left), right(right){
        rows_left = left.size();
        rows_right = right.size();

        if (rows_left == 0 || rows_right == 0) {
            cols_left = 0;
            cols_right = 0;
        } else {
            cols_left = left[0].size();
            cols_right = right[0].size();
        }
    }

    ~MultiThreadMatrixMultiplicator() {
        join_all();
    }

    Matrix run() {
        if (rows_left == 0 || rows_right == 0 || cols_left == 0 || cols_right == 0) {
            throw std::invalid_argument("One of matrices is empty");
        }

        if (cols_left != rows_right) {
            throw std::invalid_argument("Matrices cannot be multiplied due to incompatible dimensions");
        }

        const unsigned int part_rows = rows_left / num_threads;

        Matrix ans;
        ans.resize(rows_left, std::vector<int>(cols_right, 0));

        for (unsigned int i = 0; i < num_threads; ++i) {
            const unsigned int current_offset = i * part_rows;
            const unsigned int current_rows = (i != num_threads - 1) ? part_rows : (rows_left - current_offset);

            threads.emplace_back(&MultiThreadMatrixMultiplicator::multiplication, this, current_offset, current_rows, std::ref(ans));
        }

        join_all();

        return ans;
    }

private:
    void multiplication(const unsigned int offset, const unsigned rows, Matrix& ans) const {
        for (unsigned int i = offset; i < offset + rows; ++i)
            for (unsigned int j = 0; j < cols_right; ++j)
                for (unsigned int k = 0; k < cols_left; ++k)
                    ans[i][j] += left[i][k] * right[k][j];
    }

    void join_all() {
        for (auto& thread : threads)
            if (thread.joinable())
                thread.join();

        threads.clear();
    }
};
