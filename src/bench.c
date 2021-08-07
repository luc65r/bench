#define _GNU_SOURCE

#include <assert.h>
#include <dlfcn.h>
#include <elf.h>
#include <inttypes.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* https://flapenguin.me/elf-dt-gnu-hash */
static Elf64_Word get_nb_symbols_from_gnu_hash(uint32_t *hashtab) {
    const uint32_t nbuckets = hashtab[0];
    const uint32_t symoffset = hashtab[1];
    const uint32_t bloom_size = hashtab[2];
    //const uint32_t bloom_shift = hashtab[3];
    const uint64_t *bloom = (void *)&hashtab[4];
    const uint32_t *buckets = (void *)&bloom[bloom_size];
    const uint32_t *chain = &buckets[nbuckets];

    uint32_t last_sym = 0;
    for (uint32_t i = 0; i < nbuckets; i++)
        last_sym = MAX(buckets[i], last_sym);

    if (last_sym < symoffset)
        return symoffset;

    while (!(chain[last_sym - symoffset] & 1))
        last_sym++;

    return last_sym;
}

static void retrieve_functions(Elf64_Dyn *dyn, Vec *functions) {
    char *strtab = NULL;
    Elf64_Sym *symtab = NULL;
    Elf64_Word sym_nb = 0;
        
    for (; dyn->d_tag != DT_NULL; dyn++) {
        switch (dyn->d_tag) {
        case DT_HASH:
            /* https://refspecs.linuxfoundation.org/elf/gabi4+/ch5.dynamic.html#hash */
            sym_nb = ((Elf64_Word *)dyn->d_un.d_ptr)[1];
            break;

        case DT_GNU_HASH:
            sym_nb = get_nb_symbols_from_gnu_hash((uint32_t *)dyn->d_un.d_ptr);
            break;

        case DT_STRTAB:
            strtab = (char *)dyn->d_un.d_ptr;
            break;

        case DT_SYMTAB:
            symtab = (Elf64_Sym *)dyn->d_un.d_ptr;
            break;
        }
    }

    assert(symtab != NULL);
    assert(strtab != NULL);

    for (Elf64_Word j = 0; j < sym_nb; j++) {
        char *name = &strtab[symtab[j].st_name];
        printf("fn: %s\n", name);
        /*
        vec_push_back(functions, name);
        */
    }    
}

int main(int argc, char **argv) {
    Vec *functions = vec_init();
    assert(functions != NULL);

    for (int i = 1; i < argc; i++) {
        void *handle = dlopen(argv[i], RTLD_LAZY);
        struct link_map *lm;
        dlinfo(handle, RTLD_DI_LINKMAP, &lm);
        printf("name: %s\n", lm->l_name);
        retrieve_functions(lm->l_ld, NULL);
    }
    /*
    for (size_t i = 0; i < functions->len; i++)
        printf("%s\n", (char *)vec_get(functions, i));
    */

    return 0;
}
