#include "sync/mutex.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <system_error>

namespace syncobj {
namespace {

constexpr uint32_t FREE = 0;
constexpr uint32_t LOCKED = 1;

} // namespace

void Mutex::lock() {
    while (true) {
        uint32_t expected = FREE;
        if (state_.compare_exchange_strong(expected, LOCKED,
                                           std::memory_order::acquire,
                                           std::memory_order::relaxed)) {
            return;
        }

        futex_state(FUTEX_WAIT, LOCKED);
    }
}

void Mutex::unlock() {
    state_.store(FREE, std::memory_order::release);
    futex_state(FUTEX_WAKE, 1);
}

template <typename... Args> void Mutex::futex_state(Args... args) {
    if (syscall(SYS_futex, reinterpret_cast<uint32_t*>(&state_), args...) < 0) {
        throw std::system_error(errno, std::system_category(), "futex failed");
    }
}

} // namespace syncobj
