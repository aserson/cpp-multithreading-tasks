#include "task-09.h"

#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

// Тест 1: Проверка, что элементы добавляются в очередь корректно
void PushElementsTest() {
    std::cout << "PushElementsTest... ";

    ConcurrentFIFOQueue<int> queue(5);  // Очередь с лимитом на 5 элементов

    queue.push(1);
    queue.push(2);
    queue.push(3);

    assert(queue.pop() == 1);  // Первый элемент — 1
    assert(queue.pop() == 2);  // Второй элемент — 2
    assert(queue.pop() == 3);  // Третий элемент — 3

    std::cout << "Done!" << std::endl;
}

// Тест 2: Проверка корректности извлечения элементов в порядке FIFO
void PopElementsTest() {
    std::cout << "PopElementsTest... ";
    ConcurrentFIFOQueue<int> queue(5);  // Очередь с лимитом на 5 элементов

    queue.push(10);
    queue.push(20);
    queue.push(30);

    assert(queue.pop() == 10);  // Первый элемент — 10
    assert(queue.pop() == 20);  // Второй элемент — 20
    assert(queue.pop() == 30);  // Третий элемент — 30

    std::cout << "Done!" << std::endl;
}

// Тест 3: Проверка блокировки при переполнении очереди
void PushBlockingTest() {
    std::cout << "PushBlockingTest... ";
    ConcurrentFIFOQueue<int> queue(2);  // Лимит на 2 элемента

    queue.push(1);
    queue.push(2);

    std::thread t([&queue]() {
        // Попробуем вставить элемент и должны дождаться, когда место освободится
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        queue.pop();  // Удаляем элемент, чтобы освободить место
        });

    // Ожидаем, что push заблокируется на время
    const auto start = std::chrono::steady_clock::now();
    queue.push(3);  // Этот push заблокируется на некоторое время
    const auto end = std::chrono::steady_clock::now();

    const std::chrono::duration<double> diff = end - start;
    assert(diff.count() > 0.5);  // Ожидаем, что прошло хотя бы 0.5 секунды

    t.join();

    std::cout << "Done!" << std::endl;
}

// Тест 4: Проверка блокировки при пустой очереди
void PopBlockingTest() {
    std::cout << "PopBlockingTest... ";
    ConcurrentFIFOQueue<int> queue(2);  // Лимит на 2 элемента

    std::thread t([&queue]() {
        // Подождём и вставим элемент в очередь через 500 мс
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        queue.push(42);  // Добавляем элемент, чтобы разблокировать pop
        });

    // Ожидаем, что pop заблокируется на время
    const auto start = std::chrono::steady_clock::now();
    const int val = queue.pop();  // Этот pop заблокируется на некоторое время
    const auto end = std::chrono::steady_clock::now();

    const std::chrono::duration<double> diff = end - start;
    assert(diff.count() > 0.5);  // Ожидаем, что прошло хотя бы 0.5 секунды
    assert(val == 42);  // Проверяем, что извлечённое значение корректно

    t.join();

    std::cout << "Done!" << std::endl;
}

// Тест 5: Проверка тайм-аута при переполнении
void PushTimeoutTest() {
    std::cout << "PushTimeoutTest... ";
    ConcurrentFIFOQueue<int> queue(2, 1000);  // Лимит на 2 элемента и тайм-аут 1000 мс

    queue.push(1);
    queue.push(2);

    try {
        queue.push(3);
    } catch (std::runtime_error& error) {
        if (std::strcmp(error.what(), "Timeout waiting for push") == 0) {
            std::cout << "Done!" << std::endl;
            return;
        }
    }

    std::cout << "Failed!" << std::endl;
}

// Тест 6: Проверка тайм-аута при пустой очереди
void PopTimeoutTest() {
    std::cout << "PopTimeoutTest... ";
    ConcurrentFIFOQueue<int> queue(2, 1000);  // Лимит на 2 элемента и тайм-аут 1000 мс

    try {
        queue.pop();
    } catch (std::runtime_error& error) {
        if (std::strcmp(error.what(), "Timeout waiting for pop") == 0) {
            std::cout << "Done!" << std::endl;
            return;
        }
    }

    std::cout << "Failed!" << std::endl;
}

int main() {
    PushElementsTest();
    PopElementsTest();
    PushBlockingTest();
    PopBlockingTest();
    PushTimeoutTest();
    PopTimeoutTest();

    return 0;
}