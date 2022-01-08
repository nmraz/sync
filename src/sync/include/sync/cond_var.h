#include <atomic>
#include <mutex>

#include "sync/mutex.h"

namespace syncobj {

class CondVar {
public:
    constexpr CondVar() = default;

    void wait(std::unique_lock<Mutex>& lock);
    void wake_one();
    void wake_all();

private:
    void wake(uint32_t count);

    std::atomic<uint32_t> seq_{0};
};

} // namespace syncobj
