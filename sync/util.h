#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>

namespace util {

void throw_last_error(const char* what);

template <typename... Args>
[[nodiscard]] int futex(std::atomic<uint32_t>& state, int op, Args... args) {
    return syscall(SYS_futex, reinterpret_cast<uint32_t*>(&state),
                   op | FUTEX_PRIVATE_FLAG, args...);
}

} // namespace util
