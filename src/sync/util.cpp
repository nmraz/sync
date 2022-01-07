#include "util.h"

#include <system_error>

namespace util {

void throw_last_error(const char* what) {
    throw std::system_error(errno, std::system_category(), what);
}

} // namespace util
