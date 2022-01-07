#include <mutex>

#include "mutex_bench.h"

int main() {
    run_bench<std::mutex>();
    return 0;
}
