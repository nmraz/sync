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

    void lock();
    void unlock();

private:
    friend class CondVar;

    void lock_slow();

    static constexpr uint32_t STATE_FREE = 0;
    static constexpr uint32_t STATE_LOCKED = 1;
    static constexpr uint32_t STATE_WAITERS = 2;
    static constexpr uint32_t STATE_LOCKED_WAITERS =
        STATE_LOCKED | STATE_WAITERS;

    std::atomic<uint32_t> state_{STATE_FREE};
};

} // namespace syncobj
