#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

struct kv
{
    char *key;
    char *value;
    struct kv *next;
};

struct ph
{
    struct kv *kvs;
    struct ph *next;
};

struct sh
{
    struct kv *kvs;
    struct sh *next;
};

struct elfs
{
    struct kv *eh;
    struct ph *phs;
    struct sh *shs;
};

void readElfs(FILE *in, struct elfs *elfs);

int getNextLine(FILE *in, char *buffer, int size);

void scanLine(char *line, char **key, char **value);

char* findKeyValue(struct kv* kvs, char *key);

void freeElfsStruct(struct elfs *e);
