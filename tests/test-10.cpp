#include "gtest/gtest.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "task-10.h"

typedef std::vector<std::vector<int>> Matrix;

// Проверка корректности умножения двух 2x2 матриц
TEST(Task10, SmallMatrixTest) {
    Matrix A = {
        {1, 2},
        {3, 4}
    };

    Matrix B = {
        {5, 6},
        {7, 8}
    };

    Matrix expected_result = {
        {19, 22},
        {43, 50}
    };

    MultithreadMatrixMultiplicator mmm(A, B, 2);
    Matrix C = mmm.run();

    EXPECT_EQ(C, expected_result);
}

// Проверка корректности умножения двух 3x3 матриц
TEST(Task10, MediumMatrixTest) {
    Matrix A = {
        {1, 0, 2},
        {0, 1, 3},
        {4, 0, 1}
    };

    Matrix B = {
        {1, 2, 3},
        {0, 1, 2},
        {1, 0, 1}
    };

    Matrix expected_result = {
        {3, 2, 5},
        {3, 1, 5},
        {5, 8, 13}
    };

    MultithreadMatrixMultiplicator mmm(A, B, 3);
    Matrix C = mmm.run();

    EXPECT_EQ(C, expected_result);
}

// Проверка работы с матрицами 4x4
TEST(Task10, LargeMatrixTest) {
    Matrix A = {
        {2, 3, 1, 4},
        {1, 0, 0, 2},
        {3, 4, 2, 1},
        {5, 6, 1, 0}
    };

    Matrix B = {
        {1, 0, 2, 1},
        {3, 1, 4, 2},
        {2, 2, 1, 0},
        {1, 3, 3, 1}
    };

    Matrix expected_result = {
        {17, 17, 29, 12},
        {3, 6, 8, 3},
        {20, 11, 27, 12},
        {25, 8,	35,	17}
    };

    MultithreadMatrixMultiplicator mmm(A, B, 4);
    Matrix C = mmm.run();

    EXPECT_EQ(C, expected_result);
}

// Проверка работы с матрицами 1x1
TEST(Task10, OneByOneMatrixTest) {
    Matrix A = { {5} };
    Matrix B = { {3} };
    Matrix expected_result = { {15} };

    MultithreadMatrixMultiplicator mmm(A, B, 1);
    Matrix C = mmm.run();

    EXPECT_EQ(C, expected_result);
}

// Проверка работы с пустыми матрицами (ожидается исключение)
TEST(Task10, EmptyMatrixTest) {
    Matrix A = {};
    Matrix B = {};

    MultithreadMatrixMultiplicator mmm(A, B, 2);
    EXPECT_THROW({ 
        mmm.run();        
    }, std::invalid_argument);
}

// Проверка на некорректные размеры матриц (ожидается исключение)
TEST(Task10, InvalidSizeTest) {
    Matrix A = {
        {1, 2, 3},
        {4, 5, 6}
    };

    Matrix B = {
        {1, 2},
        {3, 4}
    };

    MultithreadMatrixMultiplicator mmm(A, B, 2);
    EXPECT_THROW({
        mmm.run();
        }, std::invalid_argument);
}

// Проверка корректной работы с различными потоками
TEST(Task10, MultiThreadTest) {
    Matrix A = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    Matrix B = {
        {9, 8, 7},
        {6, 5, 4},
        {3, 2, 1}
    };

    Matrix expected_result = {
        {30, 24, 18},
        {84, 69, 54},
        {138, 114, 90}
    };

    // Проверим с разным количеством потоков
    for (int num_threads = 1; num_threads <= 4; ++num_threads) {
        MultithreadMatrixMultiplicator mmm(A, B, num_threads);
        Matrix C = mmm.run();

        EXPECT_EQ(C, expected_result);
    }
}