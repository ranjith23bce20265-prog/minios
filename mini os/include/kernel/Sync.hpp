#ifndef MINI_OS_SYNC_HPP
#define MINI_OS_SYNC_HPP

#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>

namespace MiniOS {

    class Mutex {
    private:
        std::mutex mtx;
    public:
        void lock() { mtx.lock(); }
        void unlock() { mtx.unlock(); }
    };

    // Producer-Consumer Buffer
    class BoundedBuffer {
    private:
        std::queue<int> buffer;
        size_t capacity;
        std::mutex mtx;
        std::condition_variable cvFull, cvEmpty;

    public:
        explicit BoundedBuffer(size_t cap = 5) : capacity(cap) {}

        void produce(int item) {
            std::unique_lock<std::mutex> lock(mtx);
            cvFull.wait(lock, [this]() { return buffer.size() < capacity; });
            buffer.push(item);
            std::cout << "[SYNC] Produced: " << item << "\n";
            cvEmpty.notify_one();
        }

        int consume() {
            std::unique_lock<std::mutex> lock(mtx);
            cvEmpty.wait(lock, [this]() { return !buffer.empty(); });
            int item = buffer.front();
            buffer.pop();
            std::cout << "[SYNC] Consumed: " << item << "\n";
            cvFull.notify_one();
            return item;
        }
    };
}

#endif