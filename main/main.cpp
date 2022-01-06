#include <sync/mutex.h>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

void inc_many(int& val, syncobj::Mutex& mutex) {
    for (int i = 0; i < 1000000; i++) {
        mutex.lock();
        val++;
        mutex.unlock();
    }
}

int main() {
    syncobj::Mutex mutex;
    int val = 0;

    std::vector<std::thread> threads;

    for (int i = 0; i < 4; i++) {
        threads.emplace_back(inc_many, std::ref(val), std::ref(mutex));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << val << '\n';

    return 0;
}
