#include "sync/mutex.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>

#include "util.h"

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

        if (util::futex(state_, FUTEX_WAIT, LOCKED, nullptr) < 0 &&
            errno != EAGAIN) {
            util::throw_last_error("futex wait failed");
        }
    }
}

void Mutex::unlock() {
    state_.store(FREE, std::memory_order::release);
    if (util::futex(state_, FUTEX_WAKE, 1) < 0) {
        util::throw_last_error("futex wake failed");
    }
}

} // namespace syncobj
