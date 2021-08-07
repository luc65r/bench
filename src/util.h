#pragma once

#define max(a, b) ({                            \
            __auto_type _a = (a);               \
            __auto_type _b = (b);               \
            _a > _b ? _a : _b;                  \
        })

#define dbg(x, fmt, ...) ({                                     \
            __auto_type _x = (x);                               \
            fprintf(stderr, fmt, _x __VA_OPT__(,) __VA_ARGS__); \
            x;                                                  \
        })
