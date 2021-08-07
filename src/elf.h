#pragma once

#include <elf.h>

int elf_retrieve_symbols(const Elf64_Dyn *dyn, int (*callback)(const Elf64_Sym *sym, const char *strtab, void *data), void *data);
