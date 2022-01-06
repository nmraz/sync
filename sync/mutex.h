#include <atomic>
#include <cstdint>

namespace syncobj {

class Mutex {
public:
    constexpr Mutex() = default;

    void lock();
    void unlock();

private:
    template <typename... Args> void futex_state(Args... args);

    std::atomic<uint32_t> state_{0};
};

} // namespace syncobj
