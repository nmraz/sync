#include <chrono>
#include <thread>
#include <vector>

template <typename M> void run_sleep_bench() {
    using namespace std::chrono_literals;

    M mutex;
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&] {
            mutex.lock();
            std::this_thread::sleep_for(200ms);
            mutex.unlock();
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}
