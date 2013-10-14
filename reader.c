#include "reader.h"

void readElfs(FILE *in, struct elfs *elfs)
{
    char buffer[1024];
    struct kv *cur_kv;
    struct ph *cur_ph;
    struct sh *cur_sh;
    elfs = (struct elfs*)malloc(sizeof(struct elfs));
    getNextLine(in, buffer, 1024);
    if(strcmp(buffer, "[ELF HEADER]") == 0)
    {
        printf("ELF header definition not found.\n");
        exit(401);
    }

    cur_kv = elfs->eh;
    while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
    {
        cur_kv = (struct kv*)malloc(sizeof(struct kv));
        scanLine(buffer, cur_kv->key, cur_kv->value);
        cur_kv = cur_kv->next;
    }

    cur_ph = elfs->phs;
    while(strstr(buffer, "[PROGRAM HEADER") != NULL)
    {
        cur_ph = (struct ph*)malloc(sizeof(struct ph));
        cur_kv = cur_ph->kvs;
        while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
        {
            cur_kv = (struct kv*)malloc(sizeof(struct kv));
            scanLine(buffer, cur_kv->key, cur_kv->value);
            cur_kv = cur_kv->next;
        }
        cur_ph = cur_ph->next;
    }

    cur_sh = elfs->shs;
    while(strstr(buffer, "[SECTION HEADER") != NULL)
    {
        cur_sh = (struct sh*)malloc(sizeof(struct sh));
        cur_kv = cur_sh->kvs;
        while(getNextLine(in, buffer, 1024) != 0 && buffer[0] != '[')
        {
            cur_kv = (struct kv*)malloc(sizeof(struct kv));
            scanLine(buffer, cur_kv->key, cur_kv->value);
            cur_kv = cur_kv->next;
        }
        cur_sh = cur_sh->next;
    }
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

void scanLine(char *line, char *key, char *value)
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
    key = (char*) malloc(ke-ks+1);
    for(i = ks; i<ke; i++)
        key[i-ks] = line[i];
    key[i-ks] = '\0';
    while(line[i] == ' ' || line[i] == '\t')
        i++;
    vs = i;
    while(line[i] != '\n' && line[i] != '\0')
        i++;
    ve = i;
    value = (char*) malloc(ve-vs+1);
    for(i = vs; i < ve; i++)
        value[i-vs] = line[i];
    value[i-ks] = '\0';
}
