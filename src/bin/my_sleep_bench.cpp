
#include <sync/mutex.h>

#include "sleep_bench.h"

int main() {
    run_sleep_bench<syncobj::Mutex>();
    return 0;
}
