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
#include "vec.h"

struct filter_data {
    Vec *functions;
    regex_t *re;
};

struct fn_infos {
    const char *name;
    uint64_t (*addr)(uint64_t n);
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
    fni->addr = (uint64_t (*)(uint64_t))sym->st_value;

    err = vec_push_back(d->functions, fni);
    if (err != 0)
        return err;

    return 0;
}

void bench(char **files, regex_t *re) {
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

        printf("name: %s\n", lm->l_name);

        struct filter_data filter_data = {
            .functions = functions,
            .re = re,
        };

        err = elf_retrieve_symbols(lm->l_ld, &filter_sym, &filter_data);
        if (err != 0)
            error(1, err, "error while retrieving shared library symbols");
    }

    for (size_t i = 0; i < functions->len; i++) {
        struct fn_infos *fni = vec_get(functions, i);
        printf("%s\n", fni->name);
    }
}
