#include <sync/mutex.h>

#include "mutex_bench.h"

int main() {
    run_bench<syncobj::Mutex>();
    return 0;
}
