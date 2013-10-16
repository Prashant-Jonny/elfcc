#include "reader.h"

void readElfs(FILE *in, struct elfs *elfs)
{
    char buffer[1024];
    struct kv *cur_kv;
    struct ph *cur_ph;
    struct sh *cur_sh;
    getNextLine(in, buffer, 1024);
    if(strcmp(buffer, "[ELF HEADER]") == 0)
    {
        printf("ELF header definition not found.\n");
        exit(401);
    }

    elfs->eh = (struct kv*)malloc(sizeof(struct kv));
    cur_kv = elfs->eh;
    while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
    {
        scanLine(buffer, &cur_kv->key, &cur_kv->value);
        cur_kv->next = (struct kv*)malloc(sizeof(struct kv));
        cur_kv = cur_kv->next;
    }
    cur_kv->next = NULL;

    elfs->phs = (struct ph*)malloc(sizeof(struct ph));
    cur_ph = elfs->phs;
    while(strstr(buffer, "[PROGRAM HEADER") != NULL)
    {
        cur_ph->kvs = (struct kv*)malloc(sizeof(struct kv));
        cur_kv = cur_ph->kvs;
        while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
        {
            scanLine(buffer, &cur_kv->key, &cur_kv->value);
            cur_kv->next = (struct kv*)malloc(sizeof(struct kv));
            cur_kv = cur_kv->next;
        }
        cur_kv->next = NULL;
        cur_ph->next = (struct ph*)malloc(sizeof(struct ph));
        cur_ph = cur_ph->next;
    }
    cur_ph->next = NULL;
    
    elfs->shs = (struct sh*)malloc(sizeof(struct sh));
    cur_sh = elfs->shs;
    while(strstr(buffer, "[SECTION HEADER") != NULL)
    {
        cur_sh->kvs = (struct kv*)malloc(sizeof(struct kv));
        cur_kv = cur_sh->kvs;
        while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
        {
            scanLine(buffer, &cur_kv->key, &cur_kv->value);
            cur_kv->next = (struct kv*)malloc(sizeof(struct kv));
            cur_kv = cur_kv->next;
        }
        cur_kv->next = NULL;
        cur_sh->next = (struct sh*)malloc(sizeof(struct sh));
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

char* findKeyValue(struct kv* kvs, char *key)
{
    struct kv *cur_kv = kvs;
    while(cur_kv->next != NULL)
    {
        if(strcmp(cur_kv->key, key) == 0)
            return cur_kv->value;
        cur_kv = cur_kv->next;
    }
    return NULL;
}

void freeElfsStruct(struct elfs *e)
{
    struct kv *cur_kv, *next_kv;
    struct ph *cur_ph, *next_ph;
    struct sh *cur_sh, *next_sh;

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
