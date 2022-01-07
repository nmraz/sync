#include <atomic>
#include <cstdint>

namespace syncobj {

class Mutex {
public:
    constexpr Mutex() = default;

    Mutex(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;

    Mutex& operator=(const Mutex&) = delete;
    Mutex& operator=(Mutex&&) = delete;

    void lock() {
        uint32_t expected = STATE_UNLOCKED;
        if (state_.compare_exchange_weak(expected, STATE_LOCKED,
                                         std::memory_order::acquire,
                                         std::memory_order::relaxed)) {
            return;
        }

        lock_slow();
    }

    void unlock() {
        uint32_t expected = STATE_LOCKED;
        if (state_.compare_exchange_weak(expected, STATE_UNLOCKED,
                                         std::memory_order::release)) {
            return;
        }

        unlock_slow();
    }

private:
    friend class CondVar;

    void lock_slow();
    void unlock_slow();

    static constexpr uint32_t STATE_UNLOCKED = 0;
    static constexpr uint32_t STATE_LOCKED = 1;
    static constexpr uint32_t STATE_LOCKED_CHECK_WAITERS = 2;
    static constexpr uint32_t STATE_UNLOCKING = 3;

    std::atomic<uint32_t> state_{STATE_UNLOCKED};
    std::atomic<uint32_t> waiters_{0};
};

} // namespace syncobj
