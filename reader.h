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

