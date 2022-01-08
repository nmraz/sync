#include "sync/cond_var.h"

#include <errno.h>
#include <linux/futex.h>

#include <atomic>
#include <cassert>
#include <climits>
#include <cstdint>
#include <mutex>
#include <system_error>

#include "util.h"

namespace syncobj {

void CondVar::wait(std::unique_lock<Mutex>& lock) {
    uint32_t seq_val = seq_.load(std::memory_order::relaxed);

    lock.unlock();

    // Atomically re-verify `seq_` under the futex lock; any wakes that occurred
    // during the unlock will cause us not to go to sleep.
    if (util::futex(seq_, FUTEX_WAIT, seq_val, nullptr) < 0 && errno != EBUSY) {
        util::throw_last_error("futex wait failed");
    }

    lock.lock();
}

void CondVar::wake_one() {
    wake(1);
}

void CondVar::wake_all() {
    wake(INT_MAX);
}

void CondVar::wake(uint32_t count) {
    seq_.fetch_add(1, std::memory_order::relaxed);
    if (util::futex(seq_, FUTEX_WAKE, count) < 0) {
        util::throw_last_error("futex wake failed");
    }
}

} // namespace syncobj
