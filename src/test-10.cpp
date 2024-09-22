#include "task-10.h"

#include <iostream>
#include <cassert>
#include <cstring>

typedef std::vector<std::vector<int>> Matrix;

// Проверка корректности умножения двух 2x2 матриц
void SmallMatrixTest() {
    std::cout << "Small Matrix Test... ";

    const Matrix A = {
        {1, 2},
        {3, 4}
    };

    const Matrix B = {
        {5, 6},
        {7, 8}
    };

    const Matrix expected_C = {
        {19, 22},
        {43, 50}
    };

    MultiThreadMatrixMultiplicator mmm(A, B, 2);
    const Matrix computed_C = mmm.run();

    assert(computed_C == expected_C);

    std::cout << "Done!" << std::endl;
}

// Проверка корректности умножения двух 3x3 матриц
void MediumMatrixTest() {
    std::cout << "Medium Matrix Test... ";

    const Matrix A = {
        {1, 0, 2},
        {0, 1, 3},
        {4, 0, 1}
    };

    const Matrix B = {
        {1, 2, 3},
        {0, 1, 2},
        {1, 0, 1}
    };

    const Matrix expected_C = {
        {3, 2, 5},
        {3, 1, 5},
        {5, 8, 13}
    };

    MultiThreadMatrixMultiplicator mmm(A, B, 3);
    const Matrix computed_C = mmm.run();

    assert(computed_C == expected_C);

    std::cout << "Done!" << std::endl;
}

// Проверка работы с матрицами 4x4
void LargeMatrixTest() {
    std::cout << "Large Matrix Test... ";

    const Matrix A = {
        {2, 3, 1, 4},
        {1, 0, 0, 2},
        {3, 4, 2, 1},
        {5, 6, 1, 0}
    };

    const Matrix B = {
        {1, 0, 2, 1},
        {3, 1, 4, 2},
        {2, 2, 1, 0},
        {1, 3, 3, 1}
    };

    const Matrix expected_C = {
        {17, 17, 29, 12},
        {3, 6, 8, 3},
        {20, 11, 27, 12},
        {25, 8,	35,	17}
    };

    MultiThreadMatrixMultiplicator mmm(A, B, 4);
    const Matrix computed_C = mmm.run();

    assert(computed_C == expected_C);

    std::cout << "Done!" << std::endl;
}

// Проверка работы с матрицами 1x1
void OneByOneMatrixTest() {
    std::cout << "OneByOne Matrix Test... ";

    const Matrix A = { {5} };
    const Matrix B = { {3} };
    const Matrix expected_C = { {15} };

    MultiThreadMatrixMultiplicator mmm(A, B, 1);
    const Matrix computed_C = mmm.run();

    assert(computed_C == expected_C);

    std::cout << "Done!" << std::endl;
}

// Проверка работы с пустыми матрицами (ожидается исключение)
void EmptyMatrixTest() {
    std::cout << "Empty Matrix Test... ";

    const Matrix A = {};
    const Matrix B = {};

    MultiThreadMatrixMultiplicator mmm(A, B, 2);

    try {
        mmm.run();
    } catch (const std::invalid_argument& error) {
        if (std::strcmp(error.what(), "One of matrices is empty") == 0) {
            std::cout << "Done!" << std::endl;
            return;
        }
    }

    std::cout << "Failed!" << std::endl;
}

// Проверка на некорректные размеры матриц (ожидается исключение)
void InvalidSizeTest() {
    std::cout << "Invalid size test... ";

    const Matrix A = {
        {1, 2, 3},
        {4, 5, 6}
    };

    const Matrix B = {
        {1, 2},
        {3, 4}
    };

    MultiThreadMatrixMultiplicator mmm(A, B, 2);

    try {
        mmm.run();
    } catch (const std::invalid_argument& error) {
        if (std::strcmp(error.what(), "Matrices cannot be multiplied due to incompatible dimensions") == 0) {
            std::cout << "Done!" << std::endl;
            return;
        }
    }

    std::cout << "Failed!" << std::endl;
}

// Проверка корректной работы с различными потоками
void MultiThreadTest() {
    std::cout << "Multi-thread test... ";

    const Matrix A = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    const Matrix B = {
        {9, 8, 7},
        {6, 5, 4},
        {3, 2, 1}
    };

    const Matrix expected_C = {
        {30, 24, 18},
        {84, 69, 54},
        {138, 114, 90}
    };

    // Проверим с разным количеством потоков
    for (int num_threads = 1; num_threads <= 4; ++num_threads) {
        MultiThreadMatrixMultiplicator mmm(A, B, num_threads);
        Matrix computed_C = mmm.run();

        assert(computed_C == expected_C);
    }

    std::cout << "Done!" << std::endl;
}

int main() {
    SmallMatrixTest();
    MediumMatrixTest();
    LargeMatrixTest();
    OneByOneMatrixTest();
    EmptyMatrixTest();
    InvalidSizeTest();
    MultiThreadTest();

    return 0;
}