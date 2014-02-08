#include "elfcc.h"

int main(int argc, char** argv)
{
    struct Parameters param;
    int i;
    memset((void*)&param, 0, sizeof(param));
    for(i = 1; i<argc; i++)
    {
        if(strcmp(argv[i], "-d") == 0)
        {
            param.action = 1;
            i++;
            if(i<argc)
                param.in_file = argv[i];
        }
        else if(strcmp(argv[i], "-c") == 0)
        {
            param.action = 2;
            i++;
            if(i<argc)
                param.in_file = argv[i];
        }
        else if(strcmp(argv[i], "-v") == 0)
        {
            param.action = 4;
        }
        else if(i == argc - 1)
        {
            param.out_file = argv[i];
        }
    }

    if(param.action != 4 && param.in_file == NULL)
    {
        param.action = 0;
        printf("Please specify an input file.\n");
    }

    switch(param.action)
    {
        case 1:
            decompile(&param);
            return 0;
        case 2:
            compile(&param);
            return 0;
        case 4:
            version();
            return 0;
        default:
            usage();
            return 1;
    }
}

void version()
{
    printf("ELFCC 0.2\nCopyright Charles Hubain <haxelion@gmail.com> @haxelion\nReleased under GPLv3\n");
}

void usage()
{
    version();
    printf("\n");
    printf("elfcc -d file output_file: decompile an elf\n");
    printf("elfcc -c file output_file: compile into an elf\n");
}


void compile(struct Parameters *p)
{
    char buffer[1024];
    char *elfs_base_path, *machine;
    uint32_t i;
    FILE *section_file;
    struct Ph *cur_ph;
    struct Sh *cur_sh;
    struct Elfs *elfs;
    struct PreElf *pre_elf;

    elfs = (struct Elfs*)malloc(sizeof(struct Elfs));
    pre_elf = (struct PreElf*)malloc(sizeof(struct PreElf));
    elfs_base_path = (char*) malloc(strlen(p->in_file)+1);
    strcpy(elfs_base_path, p->in_file);
    *strrchr(elfs_base_path, '.') = '\0';
    p->elfs = fopen(p->in_file, "r");
    if(p->elfs == NULL)
    {
        printf("Can't open Elfs input file.\n");
        exit(101);
    }
    p->elf = fopen(p->out_file, "w");
    if(p->elf == NULL)
    {
        printf("Can't create output elf file.\n");
        exit(102);
    }
    readElfs(p->elfs, elfs);
    machine = findKeyValue(elfs->eh, "Machine");
    cur_ph = elfs->phs;
    pre_elf->phdr_number = 0;
    while(cur_ph->next != NULL)
    {
        cur_ph = cur_ph->next;
        pre_elf->phdr_number++;
    }
    cur_sh = elfs->shs;
    pre_elf->shdr_number = 0;
    while(cur_sh->next != NULL)
    {
        cur_sh = cur_sh->next;
        pre_elf->shdr_number++;
    }
    pre_elf->section_size = (uint64_t*)calloc(pre_elf->shdr_number, sizeof(uint64_t));
    pre_elf->section_data = (void**)calloc(pre_elf->shdr_number, sizeof(void*));
    pre_elf->section_total_size = 0;
    for(i = 0; i< pre_elf->shdr_number; i++)
    {
        snprintf(buffer, 1024, "%s.%x", elfs_base_path, i);
        section_file = fopen(buffer, "r");
        fseek(section_file, 0, SEEK_END);
        pre_elf->section_size[i] = ftell(section_file);
        pre_elf->section_total_size += pre_elf->section_size[i];
        pre_elf->section_data[i] = (void*)malloc(pre_elf->section_size[i]);
        fseek(section_file, 0, SEEK_SET);
        fread(pre_elf->section_data[i], pre_elf->section_size[i], 1, section_file);
        fclose(section_file);
    }
    if(strcmp(machine, "EM_386")==0 || strcmp(machine, "EM_PPC")==0 || strcmp(machine, "EM_ARM")==0)
        compile32(p->elf, elfs, pre_elf);
    else if(strcmp(machine, "EM_PPC64")==0 || strcmp(machine, "EM_IA64")==0 || strcmp(machine, "EM_X86_64")==0 || strcmp(machine, "EM_AARCH64")==0)
        compile64(p->elf, elfs, pre_elf);
    else
    {
        printf("Machine type %s not understood.\n", machine);
        exit(503);
    }
    fclose(p->elfs);
    fclose(p->elf);
    freeElfsStruct(elfs);
}

void compile32(FILE *out, struct Elfs *elfs, struct PreElf *pre_elf)
{
    
}

void compile64(FILE *out, struct Elfs *elfs, struct PreElf *pre_elf)
{
    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    struct Ph *cur_ph;
    struct Sh *cur_sh;
    uint32_t i;
    char *value;
    uint64_t offset;
    ehdr = (Elf64_Ehdr*)malloc(sizeof(Elf64_Ehdr));
    phdr = (Elf64_Phdr*)calloc(pre_elf->phdr_number, sizeof(Elf64_Phdr));
    shdr = (Elf64_Shdr*)calloc(pre_elf->shdr_number, sizeof(Elf64_Shdr));

    ehdr->e_shstrndx = strtol(findKeyValue(elfs->eh, "Section header string index"), NULL, 16);
    cur_sh = elfs->shs;
    offset = sizeof(Elf64_Ehdr) + pre_elf->phdr_number*sizeof(Elf64_Phdr);
    for(i = 0; i<pre_elf->shdr_number; i++)
    {
        shdr[i].sh_name = findString(pre_elf->section_data[ehdr->e_shstrndx], pre_elf->section_size[ehdr->e_shstrndx], findKeyValue(cur_sh->kvs, "Name"));
        shdr[i].sh_type = parseSType(findKeyValue(cur_sh->kvs, "Type"));
        shdr[i].sh_flags = parseSFlags(findKeyValue(cur_sh->kvs, "Flags"));
        shdr[i].sh_addr = strtol(findKeyValue(cur_sh->kvs, "Address"), NULL, 16);
        shdr[i].sh_size = strtol(findKeyValue(cur_sh->kvs, "Size"), NULL, 16);
        shdr[i].sh_link = strtol(findKeyValue(cur_sh->kvs, "Link"), NULL, 16);
        shdr[i].sh_info = strtol(findKeyValue(cur_sh->kvs, "Info"), NULL, 16); 
        shdr[i].sh_addralign = strtol(findKeyValue(cur_sh->kvs, "Align"), NULL, 16); 
        shdr[i].sh_entsize = strtol(findKeyValue(cur_sh->kvs, "Entry size"), NULL, 16); 
        if(shdr[i].sh_addralign != 0 && offset % shdr[i].sh_addralign != 0)
        {
            printf("%s : Align = %lld - Current offset = %lld - Shift = %lld\n", findKeyValue(cur_sh->kvs, "Name"), shdr[i].sh_addralign, offset, shdr[i].sh_addralign - (offset % shdr[i].sh_addralign));
            offset += shdr[i].sh_addralign - (offset % shdr[i].sh_addralign);
        }
        if(shdr[i].sh_type == SHT_NULL)
            shdr[i].sh_offset = 0;
        else
            shdr[i].sh_offset = offset;
        offset += pre_elf->section_size[i];
        cur_sh = cur_sh->next;
    }

    parseIdent(findKeyValue(elfs->eh, "Magic"), ehdr->e_ident);
    ehdr->e_type = parseEType(findKeyValue(elfs->eh, "Type"));
    ehdr->e_machine = parseMachine(findKeyValue(elfs->eh, "Machine"));
    ehdr->e_version = parseVersion(findKeyValue(elfs->eh, "Version"));
    ehdr->e_entry = strtol(findKeyValue(elfs->eh, "Entry point"), NULL, 16);
    ehdr->e_phoff = sizeof(Elf64_Ehdr);
    ehdr->e_shoff = offset;
    ehdr->e_flags = 0;
    ehdr->e_ehsize = sizeof(Elf64_Ehdr);
    ehdr->e_phentsize = sizeof(Elf64_Phdr);
    ehdr->e_phnum = pre_elf->phdr_number;
    ehdr->e_shentsize = sizeof(Elf64_Shdr);
    ehdr->e_shnum = pre_elf->shdr_number;

    cur_ph = elfs->phs;
    for(i = 0; i<pre_elf->phdr_number; i++)
    {
        uint32_t si;
        phdr[i].p_type = parsePType(findKeyValue(cur_ph->kvs, "Type"));
        phdr[i].p_flags = parsePFlags(findKeyValue(cur_ph->kvs, "Flags"));
        value = findKeyValue(cur_ph->kvs, "Start");
        si = strtol(strstr(value, "Section")+7, NULL, 16);
        offset = strtol(strchr(value, '+')+1, NULL, 16);
        printf("%x %x\n", si, offset);
        phdr[i].p_offset = shdr[si].sh_offset+offset;
        value = findKeyValue(cur_ph->kvs, "End");
        si = strtol(strstr(value, "Section")+7, NULL, 16);
        offset = strtol(strchr(value, '+')+1, NULL, 16);
        printf("%x %x\n", si, offset);
        phdr[i].p_filesz = shdr[si].sh_offset+offset-phdr[i].p_offset;
        phdr[i].p_vaddr = strtol(findKeyValue(cur_ph->kvs, "Virtual address"), NULL, 16);
        phdr[i].p_paddr = strtol(findKeyValue(cur_ph->kvs, "Physical address"), NULL, 16);
        phdr[i].p_align = strtol(findKeyValue(cur_ph->kvs, "Align"), NULL, 16);
        phdr[i].p_memsz = phdr[i].p_filesz + strtol(findKeyValue(cur_ph->kvs, "Memory size delta"), NULL, 16);
        cur_ph = cur_ph->next;
    }

    fwrite(ehdr, sizeof(Elf64_Ehdr), 1, out);
    for(i = 0; i<pre_elf->phdr_number; i++)
        fwrite(&(phdr[i]), sizeof(Elf64_Phdr), 1, out);
    for(i = 0; i<pre_elf->shdr_number; i++)
    {
        fseek(out, shdr[i].sh_offset, SEEK_SET);
        fwrite(pre_elf->section_data[i], pre_elf->section_size[i], 1, out);
    }
    for(i = 0; i<pre_elf->shdr_number; i++)
        fwrite(&(shdr[i]), sizeof(Elf64_Shdr), 1, out);
}


void decompile(struct Parameters *p)
{
    char *elfs_path;
    int bits;
    elfs_path = (char*) malloc(strlen(p->out_file)+6);
    strcpy(elfs_path, p->out_file);
    strcat(elfs_path, ".elfs");
    p->elf = fopen(p->in_file, "r");
    if(p->elf == NULL)
    {
        printf("Can't open elf input file.\n");
        exit(101);
    }
    p->elfs = fopen(elfs_path, "w");
    if(p->elfs == NULL)
    {
        printf("Can't create output Elfs file.\n");
        exit(102);
    }
    bits = getBits(p->elf);
    if(bits == 32)
        decompile32(p);
    else if(bits == 64)
        decompile64(p);

    free(elfs_path);
    fclose(p->elf);
    fclose(p->elfs);
}

int getBits(FILE *elf)
{
    uint16_t arch;
    fseek(elf, EI_NIDENT+2, SEEK_SET);
    fread(&arch, 2, 1, elf);
    switch(arch)
    {
        case EM_386:
        case EM_PPC:
        case EM_ARM:
            arch = 32;
            break;
        case EM_PPC64:
        case EM_IA_64:
        case EM_X86_64:
        case EM_AARCH64:
            arch = 64;
            break;
        default:
            printf("Unknown architecture, assuming 32 bits.\n");
    }
    return arch;
}

void decompile32(struct Parameters *p)
{
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;
    Elf32_Shdr *shdr;
    char buffer[1024];
    FILE *section_out;
    char **section_names;
    uint32_t string_section_index;
    void **sections;
    unsigned int c, i, j;
    //ELF Header
    ehdr = (Elf32_Ehdr*) malloc(sizeof(Elf32_Ehdr));
    fseek(p->elf, 0, SEEK_SET);
    if(fread((void*)ehdr, sizeof(Elf32_Ehdr), 1, p->elf) == 0)
    {
        printf("No ELF32 header found.\n");
        exit(301);
    }
    //ELF Program Header
    phdr = (Elf32_Phdr*) calloc(ehdr->e_phnum, sizeof(Elf32_Phdr));
    fseek(p->elf, ehdr->e_phoff, SEEK_SET);
    c = fread((void*)phdr, sizeof(Elf32_Phdr), ehdr->e_phnum, p->elf);
    printf("%d program headers read.\n", c);
    if(c != ehdr->e_phnum)
    {
        printf("ELF32 program header %u is missing.\n", c);
        exit(302);
    }
    //ELF Section Header
    shdr = (Elf32_Shdr*) calloc(ehdr->e_shnum + 1, sizeof(Elf32_Shdr));
    fseek(p->elf, ehdr->e_shoff, SEEK_SET);
    c = fread((void*)shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, p->elf);
    printf("%d section headers read.\n", c);
    if(c != ehdr->e_shnum)
    {
        printf("ELF32 section header %u is missing.\n", c);
        exit(303);
    }
    //Sections
    sections = (void**) calloc(ehdr->e_shnum, sizeof(void*));
    //Fill a guard section offset
    fseek(p->elf, 0, SEEK_END);
    shdr[ehdr->e_shnum].sh_offset = ftell(p->elf);
    for(i = 0; i<ehdr->e_shnum; i++)
    {
        unsigned int size = shdr[i].sh_size;
        if(size > (shdr[i+1].sh_offset - shdr[i].sh_offset))
            size = shdr[i+1].sh_offset - shdr[i].sh_offset;
        sections[i] = (void*) malloc(size);
        fseek(p->elf, shdr[i].sh_offset, SEEK_SET);
        if(size != 0 && fread((void*) sections[i], size, 1, p->elf) == 0)
        {
            printf("Elf parsing error: section %u at offset %llu  of size %llu could not be read.\n", i, shdr[i].sh_offset, size);
            exit(304);
        }
        snprintf(buffer, 1024, "%s.%x", p->out_file, i);
        section_out = fopen(buffer, "w");
        if(section_out == NULL)
        {
            printf("Couldn't create file %s to dump section %u.\n", buffer, i);
            exit(305);
        }
        fwrite(sections[i], size, 1, section_out);
        fclose(section_out);
    }
    //Get section string table index
    //It's the worst part of the ELF format
    if(ehdr->e_shstrndx<SHN_LORESERVE)
        string_section_index = ehdr->e_shstrndx;
    else
        string_section_index = shdr[0].sh_link;

    writeEhdr32(p->elfs, ehdr);
    writePhdr32(p->elfs, phdr, shdr, ehdr->e_phnum, ehdr->e_shnum);
    writeShdr32(p->elfs, shdr, ehdr->e_shnum, (char*)sections[string_section_index], shdr[string_section_index].sh_size);
}

    
void decompile64(struct Parameters *p)
{
    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    char buffer[1024];
    FILE *section_out;
    char **section_names;
    uint32_t string_section_index;
    void **sections;
    unsigned int c, i, j;
    //ELF Header
    ehdr = (Elf64_Ehdr*) malloc(sizeof(Elf64_Ehdr));
    fseek(p->elf, 0, SEEK_SET);
    if(fread((void*)ehdr, sizeof(Elf64_Ehdr), 1, p->elf) == 0)
    {
        printf("No ELF64 header found.\n");
        exit(301);
    }
    //ELF Program Header
    phdr = (Elf64_Phdr*) calloc(ehdr->e_phnum, sizeof(Elf64_Phdr));
    fseek(p->elf, ehdr->e_phoff, SEEK_SET);
    c = fread((void*)phdr, sizeof(Elf64_Phdr), ehdr->e_phnum, p->elf);
    printf("%d program headers read.\n", c);
    if(c != ehdr->e_phnum)
    {
        printf("ELF64 program header %u is missing.\n", c);
        exit(302);
    }
    //ELF Section Header
    shdr = (Elf64_Shdr*) calloc(ehdr->e_shnum + 1, sizeof(Elf64_Shdr));
    fseek(p->elf, ehdr->e_shoff, SEEK_SET);
    c = fread((void*)shdr, sizeof(Elf64_Shdr), ehdr->e_shnum, p->elf);
    printf("%d section headers read.\n", c);
    if(c != ehdr->e_shnum)
    {
        printf("ELF64 section header %u is missing.\n", c);
        exit(303);
    }
    //Sections
    sections = (void**) calloc(ehdr->e_shnum, sizeof(void*));
    //Fill a guard section offset
    fseek(p->elf, 0, SEEK_END);
    shdr[ehdr->e_shnum].sh_offset = ftell(p->elf);
    for(i = 0; i<ehdr->e_shnum; i++)
    {
        unsigned long long size = shdr[i].sh_size;
        if(size > (shdr[i+1].sh_offset - shdr[i].sh_offset))
            size = shdr[i+1].sh_offset - shdr[i].sh_offset;
        sections[i] = (void*) malloc(size);
        fseek(p->elf, shdr[i].sh_offset, SEEK_SET);
        if(size != 0 && fread((void*) sections[i], size, 1, p->elf) == 0)
        {
            printf("Elf parsing error: section %u at offset %llu  of size %llu could not be read.\n", i, shdr[i].sh_offset, size);
            exit(304);
        }
        snprintf(buffer, 1024, "%s.%x", p->out_file, i);
        section_out = fopen(buffer, "w");
        if(section_out == NULL)
        {
            printf("Couldn't create file %s to dump section %u.\n", buffer, i);
            exit(305);
        }
        fwrite(sections[i], size, 1, section_out);
        fclose(section_out);
    }
    //Get section string table index
    //It's the worst part of the ELF format
    if(ehdr->e_shstrndx<SHN_LORESERVE)
        string_section_index = ehdr->e_shstrndx;
    else
        string_section_index = shdr[0].sh_link;

    writeEhdr64(p->elfs, ehdr);
    writePhdr64(p->elfs, phdr, shdr, ehdr->e_phnum, ehdr->e_shnum);
    writeShdr64(p->elfs, shdr, ehdr->e_shnum, (char*)sections[string_section_index], shdr[string_section_index].sh_size);
}
