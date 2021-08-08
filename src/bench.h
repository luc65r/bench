#pragma once

#include <inttypes.h>
#include <regex.h>

typedef struct {
    uint64_t runs;
    uint64_t fn_arg;
    regex_t *re;
} BenchSettings;

void bench(char **files, BenchSettings *settings);
