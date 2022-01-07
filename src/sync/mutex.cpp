#include "sync/mutex.h"

#include <atomic>
#include <cerrno>
#include <cstddef>
#include <thread>

#include "util.h"

namespace syncobj {

void Mutex::lock_slow() {
    constexpr int SPIN_LIMIT = 40;

    uint32_t cur_state = state_.load(std::memory_order::relaxed);

    // Start by spinning a bit, as long as no one else is sleeping on
    // the lock (in which case it would be beneficial to join the queue
    // ourselves).
    for (int spin = 0; spin < SPIN_LIMIT; spin++) {
        if (cur_state == STATE_UNLOCKED &&
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
        uint32_t expected = STATE_UNLOCKED;
        if (state_.compare_exchange_weak(expected, desired,
                                         std::memory_order::acquire,
                                         std::memory_order::relaxed)) {
            return;
        }

        // If we're not the first waiter (but end up grabbing the lock first),
        // we must still remember to check other waiters upon unlock.
        // Conservatively assume that we may have other waiters here; the exact
        // status is tracked in `waiters_`.
        desired = STATE_LOCKED_WAITERS;

        // Surround the wait state with appropriate increments and
        // decrements of `waiters_`. These results are published to
        // `unlock()` when we successfully set our state to
        // `STATE_LOCKED_WAITERS` below.

        waiters_.fetch_add(1, std::memory_order::relaxed);

        // At this point, we want to get into the `STATE_LOCKED_WAITERS` state
        // for the `futex` call below; it doesn't matter if we were already here
        // or if we got here from `STATE_LOCKED`. What _does_ matter is that the
        // sleep should be avoided if we came from a different state, as the
        // mutex is either unlocked or being unlocked now.
        //
        // The transition to `STATE_LOCKED_WAITERS` also has the important
        // side-effect of publishing our increment of `waiters_` (the CAS is
        // performed as a release store, and also participates in the release
        // sequences of other CASes where necessary); we can only sleep if we
        // know that the publication has succeeded and that we will definitely
        // be woken up.

        bool can_sleep = false;

        expected = state_.load(std::memory_order::relaxed);
        if (expected == STATE_LOCKED || expected == STATE_LOCKED_WAITERS) {
            // Yes, the CAS really is necessary even when `state_` is already
            // `STATE_LOCKED_WAITERS`: the release write here synchronizes-with
            // the fence in `unlock_slow()` and makes the write to `waiters_`
            // visible.
            can_sleep = state_.compare_exchange_weak(
                expected, STATE_LOCKED_WAITERS, std::memory_order::release);
        }

        if (can_sleep) {
            if (util::futex(state_, FUTEX_WAIT, STATE_LOCKED_WAITERS, nullptr) <
                    0 &&
                errno != EAGAIN) {
                util::throw_last_error("futex wait failed");
            }
        }

        waiters_.fetch_sub(1, std::memory_order::relaxed);
    }
}

void Mutex::unlock_slow() {
    bool should_wake = false;

    uint32_t prev_state =
        state_.exchange(STATE_UNLOCKING, std::memory_order::relaxed);

    if (prev_state == STATE_LOCKED_WAITERS) {
        // Syncronize-with (potential) release store publishing `waiters_`.
        std::atomic_thread_fence(std::memory_order::acquire);
        should_wake = waiters_.load(std::memory_order::relaxed) > 0;
    }

    state_.store(STATE_UNLOCKED, std::memory_order::release);

    // Note: at this point the lock has been unlocked, meaning that someone else
    // could have stepped in and destroyed us! `*this` must not be accessed
    // beyond this point.

    if (should_wake) {
        // This doesn't actually access `*this` as the `futex` wake call only
        // uses the pointer as a key.
        if (util::futex(state_, FUTEX_WAKE, 1) < 0) {
            util::throw_last_error("futex wake failed");
        }
    }
}

} // namespace syncobj
