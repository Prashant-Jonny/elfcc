#include "reader.h"

void readElfs(FILE *in, struct Elfs *elfs)
{
    char buffer[1024];
    struct Kv *cur_kv;
    struct Ph *cur_ph;
    struct Sh *cur_sh;
    getNextLine(in, buffer, 1024);
    if(strcmp(buffer, "[ELF HEADER]") == 0)
    {
        printf("ELF header definition not found.\n");
        exit(401);
    }

    elfs->eh = (struct Kv*)malloc(sizeof(struct Kv));
    cur_kv = elfs->eh;
    while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
    {
        scanLine(buffer, &cur_kv->key, &cur_kv->value);
        cur_kv->next = (struct Kv*)malloc(sizeof(struct Kv));
        cur_kv = cur_kv->next;
    }
    cur_kv->next = NULL;

    elfs->phs = (struct Ph*)malloc(sizeof(struct Ph));
    cur_ph = elfs->phs;
    while(strstr(buffer, "[PROGRAM HEADER") != NULL)
    {
        cur_ph->kvs = (struct Kv*)malloc(sizeof(struct Kv));
        cur_kv = cur_ph->kvs;
        while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
        {
            scanLine(buffer, &cur_kv->key, &cur_kv->value);
            cur_kv->next = (struct Kv*)malloc(sizeof(struct Kv));
            cur_kv = cur_kv->next;
        }
        cur_kv->next = NULL;
        cur_ph->next = (struct Ph*)malloc(sizeof(struct Ph));
        cur_ph = cur_ph->next;
    }
    cur_ph->next = NULL;
    
    elfs->shs = (struct Sh*)malloc(sizeof(struct Sh));
    cur_sh = elfs->shs;
    while(strstr(buffer, "[SECTION HEADER") != NULL)
    {
        cur_sh->kvs = (struct Kv*)malloc(sizeof(struct Kv));
        cur_kv = cur_sh->kvs;
        while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
        {
            scanLine(buffer, &cur_kv->key, &cur_kv->value);
            cur_kv->next = (struct Kv*)malloc(sizeof(struct Kv));
            cur_kv = cur_kv->next;
        }
        cur_kv->next = NULL;
        cur_sh->next = (struct Sh*)malloc(sizeof(struct Sh));
        cur_sh = cur_sh->next;
    }
    cur_sh->next = NULL;
}

int getNextLine(FILE *in, char *buffer, int size)
{
    int discard = 1, i;
    while(discard)
    {
        if(fgets(buffer, size, in) == NULL)
            return 0;
        for(i = 0; discard && buffer[i] != '\0'; i++)
            if(buffer[i] > 'A')
                discard = 0;
    }
    return 1;
}

void scanLine(char *line, char **key, char **value)
{
    int i = 0, state = 0;
    int ks, ke, vs, ve;
    while(line[i] == ' ' || line[i] == '\t')
        i++;
    ks = i;
    while(line[i] != ':')
    {
        if(line[i] == '\0')
            return;
        i++;
    }
    ke = i;
    *key = (char*) malloc(ke-ks+1);
    for(i = ks; i<ke; i++)
        (*key)[i-ks] = line[i];
    (*key)[i-ks] = '\0';
    i++;
    while(line[i] == ' ' || line[i] == '\t')
        i++;
    vs = i;
    while(line[i] != '\n' && line[i] != '\0')
        i++;
    ve = i;
    *value = (char*) malloc(ve-vs+1);
    for(i = vs; i < ve; i++)
        (*value)[i-vs] = line[i];
    (*value)[i-ks] = '\0';
}

char* findKeyValue(struct Kv* kvs, char *key)
{
    struct Kv *cur_kv = kvs;
    while(cur_kv->next != NULL)
    {
        if(strcmp(cur_kv->key, key) == 0)
            return cur_kv->value;
        cur_kv = cur_kv->next;
    }
    printf("Key %s not found in elfs file.\n", key);
    exit(501);
    return NULL;
}

uint64_t findString(void *section, uint64_t size, char *string)
{
    uint64_t i, index = -1;
    int matching = 0;
    for(i = 0; i<size; i++)
    {
        if(matching)
        {
            if(((char*)section)[i] == '\0' && string[i-index] == '\0')
                break;
            else if(((char*)section)[i] != string[i-index])
                matching = 0;
        }
        else if(((char*)section)[i] == string[0])
        {
            matching = 1;
            index = i;
        }
    }
    if(index == -1)
    {
        printf("String %s not found in string table.\n", string);
        exit(502);
    }
    else if(index > UINT32_MAX)
    {
        printf("String %s is located at a too high index (wtf?) in the string table.\n", string);
        exit(503);
    }
    return index;
}
void freeElfsStruct(struct Elfs *e)
{
    struct Kv *cur_kv, *next_kv;
    struct Ph *cur_ph, *next_ph;
    struct Sh *cur_sh, *next_sh;

    next_kv = e->eh;
    while(next_kv != NULL)
    {
        cur_kv = next_kv;
        next_kv = cur_kv->next;
        free(cur_kv->key);
        free(cur_kv->value);
        free(cur_kv);
    }
    
    next_ph = e->phs;
    while(next_ph != NULL)
    {
        cur_ph = next_ph;
        next_ph = cur_ph->next;
        next_kv = cur_ph->kvs;
        while(next_kv != NULL)
        {
            cur_kv = next_kv;
            next_kv = cur_kv->next;
            free(cur_kv->key);
            free(cur_kv->value);
            free(cur_kv);
        }
        free(cur_ph);
    }
    
    next_sh = e->shs;
    while(next_sh != NULL)
    {
        cur_sh = next_sh;
        next_sh = cur_sh->next;
        next_kv = cur_sh->kvs;
        while(next_kv != NULL)
        {
            cur_kv = next_kv;
            next_kv = cur_kv->next;
            free(cur_kv->key);
            free(cur_kv->value);
            free(cur_kv);
        }
        free(cur_sh);
    }
    
    free(e);
}

uint32_t readSType(char *type)
{
    if(strcmp(type, "SHT_NULL") == 0)
        return SHT_NULL;
    else if(strcmp(type, "SHT_PROGBITS") == 0)
        return SHT_PROGBITS;
    else if(strcmp(type, "SHT_SYMTAB") == 0)
        return SHT_SYMTAB;
    else if(strcmp(type, "SHT_STRTAB") == 0)
        return SHT_STRTAB;
    else if(strcmp(type, "SHT_RELA") == 0)
        return SHT_RELA;
    else if(strcmp(type, "SHT_HASH") == 0)
        return SHT_HASH;
    else if(strcmp(type, "SHT_DYNAMIC") == 0)
        return SHT_DYNAMIC;
    else if(strcmp(type, "SHT_NOTE") == 0)
        return SHT_NOTE;
    else if(strcmp(type, "SHT_NOBITS") == 0)
        return SHT_NOBITS;
    else if(strcmp(type, "SHT_REL") == 0)
        return SHT_REL;
    else if(strcmp(type, "SHT_SHLIB") == 0)
        return SHT_SHLIB;
    else if(strcmp(type, "SHT_DYNSYM") == 0)
        return SHT_DYNSYM;
    else if(strcmp(type, "SHT_INIT_ARRAY") == 0)
        return SHT_INIT_ARRAY;
    else if(strcmp(type, "SHT_FINI_ARRAY") == 0)
        return SHT_FINI_ARRAY;
    else if(strcmp(type, "SHT_PREINIT_ARRAY") == 0)
        return SHT_PREINIT_ARRAY;
    else if(strcmp(type, "SHT_GROUP") == 0)
        return SHT_GROUP;
    else if(strcmp(type, "SHT_SYMTAB_SHNDX") == 0)
        return SHT_SYMTAB_SHNDX;
    else if(strcmp(type, "SHT_GNU_ATTRIBUTES") == 0)
        return SHT_GNU_ATTRIBUTES;
    else if(strcmp(type, "SHT_GNU_HASH") == 0)
        return SHT_GNU_HASH;
    else if(strcmp(type, "SHT_GNU_LIBLIST") == 0)
        return SHT_GNU_LIBLIST;
    else if(strcmp(type, "SHT_GNU_verdef") == 0)
        return SHT_GNU_verdef;
    else if(strcmp(type, "SHT_GNU_verneed") == 0)
        return SHT_GNU_verneed;
    else if(strcmp(type, "SHT_GNU_versym") == 0)
        return SHT_GNU_versym;
    else
    {
        printf("Section type %s not understood.\n", type);
        exit(504);
    }
}

uint32_t readSFlags(char *flags)
{
    uint32_t ret = 0;
    if(strchr(flags, 'A') != NULL)
        ret |= SHF_ALLOC;
    if(strchr(flags, 'E') != NULL)
        ret |= SHF_EXECINSTR;
    if(strchr(flags, 'W') != NULL)
        ret |= SHF_WRITE;
    if(strchr(flags, 'M') != NULL)
        ret |= SHF_MASKPROC;
    return ret;
}
