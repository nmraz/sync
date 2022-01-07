#include "sync/mutex.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <cstddef>
#include <thread>

#include "util.h"

namespace syncobj {

void Mutex::unlock() {
    uint32_t prev_state =
        state_.exchange(STATE_FREE, std::memory_order::release);

    if (prev_state == STATE_LOCKED_WAITERS) {
        if (util::futex(state_, FUTEX_WAKE, 1) < 0) {
            util::throw_last_error("futex wake failed");
        }
    }
}

void Mutex::lock_slow() {
    constexpr int SPIN_LIMIT = 40;

    uint32_t cur_state = state_.load(std::memory_order::relaxed);

    // Start by spinning a bit, as long as no one else is sleeping on the lock
    // (in which case it would be beneficial to join the queue ourselves).
    for (int spin = 0; spin < SPIN_LIMIT; spin++) {
        if (cur_state == STATE_FREE &&
            state_.compare_exchange_weak(cur_state, STATE_LOCKED,
                                         std::memory_order::acquire,
                                         std::memory_order::relaxed)) {
            return;
        }

        if (cur_state == STATE_LOCKED_WAITERS) {
            break;
        }

        std::this_thread::yield();
    }

    uint32_t desired = STATE_LOCKED;

    while (true) {
        uint32_t expected = STATE_FREE;
        if (state_.compare_exchange_weak(expected, desired,
                                         std::memory_order::acquire,
                                         std::memory_order::relaxed)) {
            return;
        }

        // From now on, we try to lock into a "locked with waiters" state,
        // though we don't actually know whether any other waiters will join.
        // This may be spurious, but the net result is just that unlock becomes
        // slightly less efficient.
        desired = STATE_LOCKED_WAITERS;

        // If this fails for some reason, our futex call won't sleep and we'll
        // end up trying again anyway.
        expected = STATE_LOCKED;
        state_.compare_exchange_weak(expected, STATE_LOCKED_WAITERS,
                                     std::memory_order::relaxed);

        if (util::futex(state_, FUTEX_WAIT, STATE_LOCKED_WAITERS, nullptr) <
                0 &&
            errno != EAGAIN) {
            util::throw_last_error("futex wait failed");
        }
    }
}

} // namespace syncobj
