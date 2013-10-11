#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

void writeEhdr64(FILE *out, Elf64_Ehdr *ehdr);

void writeEhdr32(FILE *out, Elf32_Ehdr *ehdr);

void writeMagic(FILE *out, char *ident);

void writeEType(FILE *out, uint32_t type);

void writeMachine(FILE *out, uint32_t machine);

void writeVersion(FILE *out, uint32_t version);

void writeShstrndx(FILE *out, uint32_t index);

void writePhdr64(FILE *out, Elf64_Phdr *phdr, Elf64_Shdr *shdr, uint32_t pcount, uint32_t scount);

void writePhdr32(FILE *out, Elf32_Phdr *phdr, Elf32_Shdr *shdr, uint32_t pcount, uint32_t scount);

void writePType(FILE *out, uint32_t type);

void writePFlags(FILE *out, uint32_t flags);

void writeShdr64(FILE *out, Elf64_Shdr *shdr, uint32_t scount, char *string_section, uint64_t size);

void writeShdr32(FILE *out, Elf32_Shdr *shdr, uint32_t scount, char *string_section, uint32_t size);

void writeSType(FILE *out, uint32_t type);

void writeSFlags(FILE *out, uint32_t flags);
