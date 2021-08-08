#define _GNU_SOURCE

#include <assert.h>
#include <inttypes.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "timer.h"

uint64_t count_cpu_cycles(void (*fn)(uint64_t n), uint64_t fn_arg) {
    /* TODO: error handling */
    static int perf_fd = 0;
    if (perf_fd == 0) {
        struct perf_event_attr attr = {
            .type = PERF_TYPE_HARDWARE,
            .size = sizeof attr,
            .config = PERF_COUNT_HW_CPU_CYCLES,
            .disabled = 1,
            .exclude_kernel = 1,
            .exclude_hv = 1,
        };
        perf_fd = (int)syscall(SYS_perf_event_open, &attr, 0, -1, -1, 0);
    }

    assert(perf_fd > 0);

    ioctl(perf_fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(perf_fd, PERF_EVENT_IOC_ENABLE, 0);

    fn(fn_arg);

    ioctl(perf_fd, PERF_EVENT_IOC_DISABLE, 0);
    uint64_t cycles;
    read(perf_fd, &cycles, sizeof cycles);

    return cycles;
}
