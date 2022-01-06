#include <atomic>

#include "sync/mutex.h"

namespace syncobj {

class CondVar {
public:
    constexpr CondVar() = default;

    void wait(Mutex& mutex);
    void wake_one();
    void wake_all();

private:
    std::atomic<uint32_t> state_{0};
};

} // namespace syncobj
