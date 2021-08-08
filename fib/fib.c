#include <inttypes.h>

uint64_t fib_recursive(uint64_t n) {
    if (n <= 1)
        return n;

    return fib_recursive(n - 2) + fib_recursive(n - 1);
}

static uint64_t tcfib(uint64_t n, uint64_t a, uint64_t b) {
    if (n == 0)
        return a;
    else if (n == 1)
        return b;
    else
        return tcfib(n - 1, b, a + b);
}

uint64_t fib_tail_recursive(uint64_t n) {
    return tcfib(n, 0, 1);
}

uint64_t fib_loop(uint64_t n) {
    uint64_t a = 0;
    uint64_t b = 1;

    while (n--) {
        uint64_t c = a + b;
        a = b;
        b = c;
    }

    return a;
}
