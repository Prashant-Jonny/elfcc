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

struct Kv
{
    char *key;
    char *value;
    struct Kv *next;
};

struct Ph
{
    struct Kv *kvs;
    struct Ph *next;
};

struct Sh
{
    struct Kv *kvs;
    struct Sh *next;
};

struct Elfs
{
    struct Kv *eh;
    struct Ph *phs;
    struct Sh *shs;
};

void readElfs(FILE *in, struct Elfs *elfs);

int getNextLine(FILE *in, char *buffer, int size);

void scanLine(char *line, char **key, char **value);

char* findKeyValue(struct Kv* kvs, char *key);

uint64_t findString(void *section, uint64_t size, char *string);

void freeElfsStruct(struct Elfs *e);

void parseIdent(char *in, unsigned char *out);

uint32_t parseEType(char *type);

uint32_t parseMachine(char *machine);

uint32_t parseVersion(char *version);

uint32_t parsePType(char *type);

uint32_t parsePFlags(char *flags);

uint32_t parseSType(char *type);

uint32_t parseSFlags(char *flags);

