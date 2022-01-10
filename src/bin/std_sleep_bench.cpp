
#include <mutex>

#include "sleep_bench.h"

int main() {
    run_sleep_bench<std::mutex>();
    return 0;
}
