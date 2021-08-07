#include <assert.h>
#include <elf.h>
#include <stdlib.h>

#include "elf.h"
#include "util.h"

static Elf64_Word get_nb_symbols_from_gnu_hash(uint32_t *hashtab) {
    /* https://flapenguin.me/elf-dt-gnu-hash */
    const uint32_t nbuckets = hashtab[0];
    const uint32_t symoffset = hashtab[1];
    const uint32_t bloom_size = hashtab[2];
    //const uint32_t bloom_shift = hashtab[3];
    const uint64_t *bloom = (void *)&hashtab[4];
    const uint32_t *buckets = (void *)&bloom[bloom_size];
    const uint32_t *chain = &buckets[nbuckets];

    uint32_t last_sym = 0;
    for (uint32_t i = 0; i < nbuckets; i++)
        last_sym = max(buckets[i], last_sym);

    if (last_sym < symoffset)
        return symoffset;

    while (!(chain[last_sym - symoffset] & 1))
        last_sym++;

    return last_sym + 1; /* FIXME: why + 1 ? */
}

int elf_retrieve_symbols(const Elf64_Dyn *dyn, int (*callback)(const Elf64_Sym *sym, const char *strtab, void *data), void *data) {
    const char *strtab = NULL;
    const Elf64_Sym *symtab = NULL;
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
        int err = callback(&symtab[j], strtab, data);
        if (err != 0)
            return err;
    }

    return 0;
}
