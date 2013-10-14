#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include "writer.h"
#include "reader.h"

struct parameters 
{
    int action;
    char *in_file;
    char *out_file;
    FILE *elf;
    FILE *elfs;
    FILE *patch;
};

void version();

void usage();

void compile(struct parameters *param);

void decompile(struct parameters *param);

int getBits(FILE *elf);

void decompile32(struct parameters *p);

void decompile64(struct parameters *p);

void compile32(struct parameters *p);
