#pragma once

uint64_t count_cpu_cycles(void (*fn)(uint64_t n), uint64_t fn_arg);
