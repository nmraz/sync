#include <sync/mutex.h>

#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using MyMutex = syncobj::Mutex;

void inc_many(int& val, MyMutex& mutex) {
    for (int i = 0; i < 1000000; i++) {
        mutex.lock();
        val++;
        mutex.unlock();
    }
}

int main() {
    MyMutex mutex;
    int val = 0;

    std::vector<std::thread> threads;

    for (int i = 0; i < 8; i++) {
        threads.emplace_back(inc_many, std::ref(val), std::ref(mutex));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << val << '\n';

    return 0;
}
