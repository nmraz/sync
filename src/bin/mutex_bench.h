#include <iostream>
#include <thread>
#include <vector>

template <typename M> void inc_many(int& val, M& mutex) {
    for (int i = 0; i < 1000000; i++) {
        mutex.lock();
        val++;
        mutex.unlock();
    }
}

template <typename M> void run_bench() {
    M mutex;
    int val = 0;

    std::vector<std::thread> threads;

    for (int i = 0; i < 8; i++) {
        threads.emplace_back(inc_many<M>, std::ref(val), std::ref(mutex));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << val << '\n';
}
