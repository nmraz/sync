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

    static constexpr uint32_t FREE = 0;
    static constexpr uint32_t LOCKED = 1;

    std::atomic<uint32_t> state_{FREE};
};

} // namespace syncobj
