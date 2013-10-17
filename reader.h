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

uint32_t readPType(char *type);

uint32_t readPFlags(char *flags);

uint32_t readSType(char *type);

uint32_t readSFlags(char *flags);

