#include "gtest/gtest.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "task-09.h"

// Тест 1: Проверка, что элементы добавляются в очередь корректно
TEST(Task09, PushElementsTest) {
    ConcurrentFIFOQueue<int> queue(5);  // Очередь с лимитом на 5 элементов

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(queue.pop(), 1);  // Первый элемент — 1
    EXPECT_EQ(queue.pop(), 2);  // Второй элемент — 2
    EXPECT_EQ(queue.pop(), 3);  // Третий элемент — 3
}

// Тест 2: Проверка корректности извлечения элементов в порядке FIFO
TEST(ConcurrentFIFOQueueTest, PopElementsTest) {
    ConcurrentFIFOQueue<int> queue(5);  // Очередь с лимитом на 5 элементов

    queue.push(10);
    queue.push(20);
    queue.push(30);

    EXPECT_EQ(queue.pop(), 10);  // Первый элемент — 10
    EXPECT_EQ(queue.pop(), 20);  // Второй элемент — 20
    EXPECT_EQ(queue.pop(), 30);  // Третий элемент — 30
}

// Тест 3: Проверка блокировки при переполнении очереди
TEST(ConcurrentFIFOQueueTest, PushBlockingTest) {
    ConcurrentFIFOQueue<int> queue(2);  // Лимит на 2 элемента

    queue.push(1);
    queue.push(2);

    std::thread t([&queue]() {
        // Попробуем вставить элемент и должны дождаться, когда место освободится
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        queue.pop();  // Удаляем элемент, чтобы освободить место
        });

    // Ожидаем, что push заблокируется на время
    auto start = std::chrono::steady_clock::now();
    queue.push(3);  // Этот push заблокируется на некоторое время
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> diff = end - start;
    EXPECT_GE(diff.count(), 0.5);  // Ожидаем, что прошло хотя бы 0.5 секунды

    t.join();
}

// Тест 4: Проверка блокировки при пустой очереди
TEST(ConcurrentFIFOQueueTest, PopBlockingTest) {
    ConcurrentFIFOQueue<int> queue(2);  // Лимит на 2 элемента

    std::thread t([&queue]() {
        // Подождём и вставим элемент в очередь через 500 мс
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        queue.push(42);  // Добавляем элемент, чтобы разблокировать pop
        });

    // Ожидаем, что pop заблокируется на время
    auto start = std::chrono::steady_clock::now();
    int val = queue.pop();  // Этот pop заблокируется на некоторое время
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> diff = end - start;
    EXPECT_GE(diff.count(), 0.5);  // Ожидаем, что прошло хотя бы 0.5 секунды
    EXPECT_EQ(val, 42);  // Проверяем, что извлечённое значение корректно

    t.join();
}

// Тест 5: Проверка тайм-аута при переполнении
TEST(ConcurrentFIFOQueueTest, PushTimeoutTest) {
    ConcurrentFIFOQueue<int> queue(2, 1000);  // Лимит на 2 элемента и тайм-аут 1000 мс

    queue.push(1);
    queue.push(2);

    EXPECT_THROW({
        queue.push(3);  // Ожидаем тайм-аут и выброс исключения
        }, std::runtime_error);
}

// Тест 6: Проверка тайм-аута при пустой очереди
TEST(ConcurrentFIFOQueueTest, PopTimeoutTest) {
    ConcurrentFIFOQueue<int> queue(2, 1000);  // Лимит на 2 элемента и тайм-аут 1000 мс

    EXPECT_THROW({
        queue.pop();  // Ожидаем тайм-аут и выброс исключения
        }, std::runtime_error);
}
