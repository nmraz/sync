#include "sync/mutex.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>

#include "util.h"

namespace syncobj {

void Mutex::lock() {
    while (true) {
        uint32_t expected = STATE_FREE;
        if (state_.compare_exchange_strong(expected, STATE_LOCKED,
                                           std::memory_order::acquire,
                                           std::memory_order::relaxed)) {
            return;
        }

        if (util::futex(state_, FUTEX_WAIT, STATE_LOCKED, nullptr) < 0 &&
            errno != EAGAIN) {
            util::throw_last_error("futex wait failed");
        }
    }
}

void Mutex::unlock() {
    state_.store(STATE_FREE, std::memory_order::release);
    if (util::futex(state_, FUTEX_WAKE, 1) < 0) {
        util::throw_last_error("futex wake failed");
    }
}

} // namespace syncobj
