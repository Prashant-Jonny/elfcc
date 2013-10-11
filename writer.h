#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

void writeEhdr64(FILE *out, Elf64_Ehdr *ehdr);

void writeMagic(FILE *out, char *ident);

void writeEType(FILE *out, uint32_t type);

void writeMachine(FILE *out, uint32_t machine);

void writeVersion(FILE *out, uint32_t version);

void writePhdr64(FILE *out, Elf64_Phdr *phdr, Elf64_Shdr *shdr, uint32_t pcount, uint32_t scount);

void writePType(FILE *out, uint32_t type);

void writePFlags(FILE *out, uint32_t flags);

void writeShdr64(FILE *out, Elf64_Shdr *shdr, uint32_t scount);

void writeSType(FILE *out, uint32_t type);

void writeSFlags(FILE *out, uint32_t flags);
