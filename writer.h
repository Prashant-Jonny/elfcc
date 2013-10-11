#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

void writeEhdr64(FILE *out, Elf64_Ehdr *ehdr);

void writeMagic(FILE *out, char *ident);

void writeEType(FILE *out, int type);

void writeMachine(FILE *out, int machine);

void writeVersion(FILE *out, int version);

void writePhdr64(FILE *out, Elf64_Phdr *phdr, Elf64_Shdr *shdr, int pcount, int scount);

void writePType(FILE *out, int type);
