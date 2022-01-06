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
    template <typename... Args>
    [[nodiscard]] int futex_state(int op, Args... args);

    std::atomic<uint32_t> state_{0};
};

} // namespace syncobj
