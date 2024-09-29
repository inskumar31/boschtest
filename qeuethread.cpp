#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdexcept>

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
        std::cout<< " In constructor:: Data member initialisation "<<std::endl;
        std::cout<< " Capacity:: "<<capacity<<std::endl;
        std::cout<< " Size:: "<<size<<std::endl;
        std::cout<< " front:: "<<front<<std::endl;
        std::cout<< " rear:: "<<rear<<std::endl;
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
        //std::cout<<" in push"<<std::endl;
        std::unique_lock<std::mutex> lock(mutex);
        if (size == capacity) {
            std::cout<< "Queue is full. "<<std::endl;
            front = (front + 1) % capacity;
            size--;
        }
        data[rear] = element;
        rear = (rear + 1) % capacity;
        //std::cout<< "Rear Position:: "<< rear <<std::endl;
        size++;
        // std::cout<<" ending push"<<std::endl;
        // std::cout<< " Push size:: "<<size<<std::endl;
        // std::cout<< " Push rear:: "<<rear<<std::endl;
        cond_var.notify_all();
    }

    /**
     * @brief Pop an element from the queue. If the queue is empty, this method will block indefinitely.
     * 
     * @return T The popped element.
     */
    T Pop() {
        std::cout<<" Welcome, I will pick and pop: " << std::endl;
        std::unique_lock<std::mutex> lock(mutex);
        std::cout<< "Waiting..." <<std::endl;
        cond_var.wait(lock, [this] { return size > 0; });
        std::cout<< "Finished."<<std::endl;
        T element = data[front];
        front = (front + 1) % capacity;
        size--;
        //std::cout<< " Size:: "<<size<<std::endl;
        //std::cout<< " front:: "<<front<<std::endl;
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
        std::cout<< " PopWithTimeout pop brother: "<<std::endl;
        T element = data[front];
        front = (front + 1) % capacity;
        size--;
        //std::cout<< " PopWithTimeout popped successfully!!"<<std::endl;
        return element;
    }

    /**
     * @brief Get the current number of elements stored in the queue.
     * 
     * @return int The current number of elements.
     */
    int Count() const {
        std::unique_lock<std::mutex> lock(mutex);
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

int main() {

    Queue<int> queue(5);

    // Writer thread
    std::thread writer([&queue]() {
        for (int i = 0; i < 10; ++i) {
            std::cout<< " Pushed Element: "<< i << std::endl;
            queue.Push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    // Reader thread (blocking pop)
    std::thread reader([&queue]() {
        for (int i = 0; i < 10; ++i) {
            try {
                int element =queue.Pop();
                element = queue.PopWithTimeout(100);
                std::cout << " Popped: " << element << std::endl;
            } catch (const std::runtime_error& excp) {
                std::cout << excp.what() << std::endl;
            }
        }
    });

    writer.join();
    reader.join();

    return 0;
}