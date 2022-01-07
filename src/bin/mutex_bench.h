#include <iostream>
#include <thread>
#include <vector>

template <typename M>
void inc_many(int& val, std::vector<int>& vals, M& mutex) {
    for (int i = 0; i < 1000000; i++) {
        mutex.lock();
        val++;
        vals.push_back(val);
        mutex.unlock();
    }
}

template <typename M> void run_bench() {
    M mutex;
    int val = 0;

    std::vector<std::thread> threads;
    std::vector<int> vals;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back(inc_many<M>, std::ref(val), std::ref(vals),
                             std::ref(mutex));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << val << ' ' << vals.size() << ' ' << '\n';
}
