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


void parseIdent(char *in, unsigned char *out)
{
    int j = 0;
    for(; *in != '\0' && j<EI_NIDENT; in++)
    {
        if(*in != ' ')
        {
            out[j] = strtol(in, &in, 16);
            j++;
        }
    }
    if(j != EI_NIDENT)
    {
        printf("Magic Number is missing %x bytes.\n", EI_NIDENT - j);
        exit(505);
    }
}

uint32_t parseEType(char *type)
{
    if(strstr(type, "ET_NONE") != NULL)
        return ET_NONE;
    else if(strstr(type, "ET_REL") != NULL)
        return ET_REL;
    else if(strstr(type, "ET_EXEC") != NULL)
        return ET_EXEC;
    else if(strstr(type, "ET_DYN") != NULL)
        return ET_DYN;
    else if(strstr(type, "ET_NUM") != NULL)
        return ET_NUM;
    else
    {
        printf("ELF type %s not understood.\n", type);
        exit(506);
    }
}

uint32_t parseMachine(char *machine)
{
    if(strstr(machine, "EM_NONE") != NULL)
        return EM_NONE;
    else if(strstr(machine, "EM_386") != NULL)
        return EM_386;
    else if(strstr(machine, "EM_PPC") != NULL)
        return EM_PPC;
    else if(strstr(machine, "EM_PPC64") != NULL)
        return EM_PPC64;
    else if(strstr(machine, "EM_ARM") != NULL)
        return EM_ARM;
    else if(strstr(machine, "EM_IA_64") != NULL)
        return EM_IA_64;
    else if(strstr(machine, "EM_X86_64") != NULL)
        return EM_X86_64;
    else if(strstr(machine, "EM_AARCH64") != NULL)
        return EM_AARCH64;
    else
    {
        printf("Machine type %s not understood.\n", machine);
        exit(507);
    }
}

uint32_t parseVersion(char *version)
{
    if(strstr(version, "EV_CURRENT") != NULL)
        return EV_CURRENT;
    else if(strstr(version, "EV_NONE") != NULL)
        return EV_NONE;
    else if(strstr(version, "EV_NUM") != NULL)
        return EV_NUM;
    else
    {
        printf("Version %s not understood.\n", version);
        exit(508);
    }
}

uint32_t parsePType(char *type)
{
    if(strstr(type, "PT_NULL") != NULL)
        return PT_NULL;
    else if(strstr(type, "PT_LOAD") != NULL)
        return PT_LOAD;
    else if(strstr(type, "PT_DYNAMIC") != NULL)
        return PT_DYNAMIC;
    else if(strstr(type, "PT_INTERP") != NULL)
        return PT_INTERP;
    else if(strstr(type, "PT_NOTE") != NULL)
        return PT_NOTE;
    else if(strstr(type, "PT_SHLIB") != NULL)
        return PT_SHLIB;
    else if(strstr(type, "PT_PHDR") != NULL)
        return PT_PHDR;
    else if(strstr(type, "PT_GNU_STACK") != NULL)
        return PT_GNU_STACK;
    else if(strstr(type, "PT_TLS") != NULL)
        return PT_TLS;
    else if(strstr(type, "PT_NUM") != NULL)
        return PT_NUM;
    else if(strstr(type, "PT_GNU_EH_FRAME") != NULL)
        return PT_GNU_EH_FRAME;
    else if(strstr(type, "PT_GNU_RELRO") != NULL)
        return PT_GNU_RELRO;
    else
    {
        printf("Program type %s not understood.\n", type);
        exit(504);
    }
}

uint32_t parsePFlags(char *flags)
{
    uint32_t ret = 0;
    if(strchr(flags, 'R') != NULL)
        ret |= PF_R;
    if(strchr(flags, 'W') != NULL)
        ret |= PF_W;
    if(strchr(flags, 'X') != NULL)
        ret |= PF_X;
    return ret;
}

uint32_t parseSType(char *type)
{
    if(strstr(type, "SHT_NULL") != NULL)
        return SHT_NULL;
    else if(strstr(type, "SHT_PROGBITS") != NULL)
        return SHT_PROGBITS;
    else if(strstr(type, "SHT_SYMTAB") != NULL)
        return SHT_SYMTAB;
    else if(strstr(type, "SHT_STRTAB") != NULL)
        return SHT_STRTAB;
    else if(strstr(type, "SHT_RELA") != NULL)
        return SHT_RELA;
    else if(strstr(type, "SHT_HASH") != NULL)
        return SHT_HASH;
    else if(strstr(type, "SHT_DYNAMIC") != NULL)
        return SHT_DYNAMIC;
    else if(strstr(type, "SHT_NOTE") != NULL)
        return SHT_NOTE;
    else if(strstr(type, "SHT_NOBITS") != NULL)
        return SHT_NOBITS;
    else if(strstr(type, "SHT_REL") != NULL)
        return SHT_REL;
    else if(strstr(type, "SHT_SHLIB") != NULL)
        return SHT_SHLIB;
    else if(strstr(type, "SHT_DYNSYM") != NULL)
        return SHT_DYNSYM;
    else if(strstr(type, "SHT_INIT_ARRAY") != NULL)
        return SHT_INIT_ARRAY;
    else if(strstr(type, "SHT_FINI_ARRAY") != NULL)
        return SHT_FINI_ARRAY;
    else if(strstr(type, "SHT_PREINIT_ARRAY") != NULL)
        return SHT_PREINIT_ARRAY;
    else if(strstr(type, "SHT_GROUP") != NULL)
        return SHT_GROUP;
    else if(strstr(type, "SHT_SYMTAB_SHNDX") != NULL)
        return SHT_SYMTAB_SHNDX;
    else if(strstr(type, "SHT_GNU_ATTRIBUTES") != NULL)
        return SHT_GNU_ATTRIBUTES;
    else if(strstr(type, "SHT_GNU_HASH") != NULL)
        return SHT_GNU_HASH;
    else if(strstr(type, "SHT_GNU_LIBLIST") != NULL)
        return SHT_GNU_LIBLIST;
    else if(strstr(type, "SHT_GNU_verdef") != NULL)
        return SHT_GNU_verdef;
    else if(strstr(type, "SHT_GNU_verneed") != NULL)
        return SHT_GNU_verneed;
    else if(strstr(type, "SHT_GNU_versym") != NULL)
        return SHT_GNU_versym;
    else
    {
        printf("Section type %s not understood.\n", type);
        exit(504);
    }
}

uint32_t parseSFlags(char *flags)
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

