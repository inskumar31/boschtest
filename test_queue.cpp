#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdexcept>
#include <gtest/gtest.h>

/**
 * @brief A thread-safe queue class template that supports multi-threaded communication.
 * 
 * @tparam T The type of elements stored in the queue.
 */
template <typename T>
class Queue {
public:
    /**
     * @brief Construct a new Queue object.
     * 
     * @param size The maximum number of elements the queue can hold.
     */
    Queue(int size) : capacity(size), size(0), front(0), rear(0) {
        data = new T[capacity];
    }

    /**
     * @brief Destroy the Queue object.
     */
    ~Queue() {
        delete[] data;
    }

    /**
     * @brief Push an element to the queue. If the queue is full, the oldest element will be dropped.
     * 
     * @param element The element to be pushed.
     */
    void Push(T element) {
        std::unique_lock<std::mutex> lock(mutex);
        if (size == capacity) {
            front = (front + 1) % capacity;
            size--;
        }
        data[rear] = element;
        rear = (rear + 1) % capacity;
        size++;
        cond_var.notify_all();
    }

    /**
     * @brief Pop an element from the queue. If the queue is empty, this method will block indefinitely.
     * 
     * @return T The popped element.
     */
    T Pop() {
        std::unique_lock<std::mutex> lock(mutex);
        cond_var.wait(lock, [this] { return size > 0; });
        T element = data[front];
        front = (front + 1) % capacity;
        size--;
        return element;
    }

    /**
     * @brief Pop an element from the queue with a timeout. If the queue is empty after the timeout, an exception is thrown.
     * 
     * @param milliseconds The timeout in milliseconds.
     * @return T The popped element.
     * @throws std::runtime_error If the queue is empty after the timeout.
     */
    T PopWithTimeout(int milliseconds) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!cond_var.wait_for(lock, std::chrono::milliseconds(milliseconds), [this] { return size > 0; })) {
            throw std::runtime_error("Timeout: Queue is empty");
        }
        T element = data[front];
        front = (front + 1) % capacity;
        size--;
        return element;
    }

    /**
     * @brief Get the current number of elements stored in the queue.
     * 
     * @return int The current number of elements.
     */
    int Count() const {
        //std::unique_lock<std::mutex> lock(mutex);
        return size;
    }

    /**
     * @brief Get the maximum number of elements the queue can hold.
     * 
     * @return int The maximum number of elements.
     */
    int Size() const {
        return capacity;
    }

private:
    T* data; ///< Pointer to the dynamically allocated array to store queue elements.
    int capacity; ///< The maximum number of elements the queue can hold.
    int size; ///< The current number of elements in the queue.
    int front; ///< The index of the front element in the queue.
    int rear; ///< The index of the rear element in the queue.
    std::mutex mutex; ///< Mutex to ensure thread safety.
    std::condition_variable cond_var; ///< Condition variable for blocking pop operations.
};

// Test fixture for Queue
class QueueTest : public ::testing::Test {
protected:
    Queue<int> queue{5};
};

// Test the Push and Pop operations
TEST_F(QueueTest, PushPop) {
    queue.Push(1);
    queue.Push(2);
    queue.Push(3);

    EXPECT_EQ(queue.Pop(), 1);
    EXPECT_EQ(queue.Pop(), 2);
    EXPECT_EQ(queue.Pop(), 3);
}

// Test the PopWithTimeout operation
TEST_F(QueueTest, PopWithTimeout) {
    queue.Push(1);
    queue.Push(2);

    EXPECT_EQ(queue.PopWithTimeout(100), 1);
    EXPECT_EQ(queue.PopWithTimeout(100), 2);

    EXPECT_THROW(queue.PopWithTimeout(100), std::runtime_error);
}

// Test the Count method
TEST_F(QueueTest, Count) {
    EXPECT_EQ(queue.Count(), 0);

    queue.Push(1);
    EXPECT_EQ(queue.Count(), 1);

    queue.Push(2);
    EXPECT_EQ(queue.Count(), 2);

    queue.Pop();
    EXPECT_EQ(queue.Count(), 1);
}

// Test the Size method
TEST_F(QueueTest, Size) {
    EXPECT_EQ(queue.Size(), 5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}