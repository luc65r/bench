#define _GNU_SOURCE

#include <assert.h>
#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <error.h>
#include <inttypes.h>
#include <link.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "elf.h"
#include "util.h"
#include "timer.h"
#include "vec.h"

struct filter_data {
    const char *library_name;
    Vec *functions;
    regex_t *re;
    void *handle;
};

struct fn_infos {
    const char *library;
    const char *name;
    void (*addr)(uint64_t n);
};

static int filter_sym(const Elf64_Sym *sym, const char *strtab, void *data) {
    struct filter_data *d = data;

    if (ELF64_ST_TYPE(sym->st_info) != STT_FUNC)
        return 0;

    const char *name = &strtab[sym->st_name];
    int err = regexec(d->re, name, 0, NULL, 0);
    if (err == REG_NOMATCH)
        return 0;
    assert(err == 0);

    struct fn_infos *fni = malloc(sizeof *fni);
    if (fni == NULL)
        return ENOMEM;

    fni->name = name;
    /* TODO: find someting more efficient than calling dlsym */
    fni->addr = dlsym(d->handle, name);
    fni->library = d->library_name;

    err = vec_push_back(d->functions, fni);
    if (err != 0)
        return err;

    return 0;
}

static uint64_t avg_cpu_cycles(size_t runs, void (*fn)(uint64_t), uint64_t n) {
    uint64_t cycles = 0;
    for (size_t i = 0; i < runs; i++) {
        cycles += count_cpu_cycles(fn, n);
    }

    return cycles / runs;
}

void bench(char **files, BenchSettings *settings) {
    Vec *functions = vec_init();
    assert(functions != NULL);

    for (; *files; files++) {
        void *handle = dlopen(*files, RTLD_LAZY);
        if (handle == NULL)
            error(1, 0, "error while opening shared library: %s", dlerror());

        struct link_map *lm;
        int err = dlinfo(handle, RTLD_DI_LINKMAP, &lm);
        if (err != 0)
            error(1, 0, "error while getting shared library informations: %s", dlerror());

        struct filter_data filter_data = {
            .library_name = lm->l_name,
            .functions = functions,
            .re = settings->re,
            .handle = handle,
        };

        err = elf_retrieve_symbols(lm->l_ld, &filter_sym, &filter_data);
        if (err != 0)
            error(1, err, "error while retrieving shared library symbols");
    }

    for (size_t i = 0; i < functions->len; i++) {
        struct fn_infos *fni = vec_get(functions, i);
        printf("%s: %s\n", fni->library, fni->name);
        printf("    %" PRIu64 "\n", avg_cpu_cycles(settings->runs, fni->addr, settings->fn_arg));
    }

    vec_deinit(functions);
}
