/*
This file is part of Elfcc.

Elfcc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Elfcc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Copyright 2014 Charles Hubain <haxelion@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include "writer.h"
#include "reader.h"

struct Parameters 
{
    int action;
    char *in_file;
    char *out_file;
    FILE *elf;
    FILE *elfs;
    FILE *patch;
};

struct PreElf
{
    uint32_t phdr_number;
    uint32_t shdr_number;
    uint64_t *section_size;
    uint64_t section_total_size;
    void **section_data;
};

void version();

void usage();

void compile(struct Parameters *p);

void compile32(FILE *out, struct Elfs *e, struct PreElf *pre_elf);

void compile64(FILE *out, struct Elfs *e, struct PreElf *pre_elf);

void decompile(struct Parameters *p);

int getBits(FILE *elf);

void decompile32(struct Parameters *p);

void decompile64(struct Parameters *p);
