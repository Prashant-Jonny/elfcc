#include "elfcc.h"

int main(int argc, char** argv)
{
    struct parameters param;
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
        else if(strcmp(argv[i], "-o") == 0)
        {
            i++;
            if(i<argc)
                param.out_file = argv[i];
        }
    }

    if(param.action != 4 && param.in_file == NULL)
    {
        param.action = 0;
        printf("Please specify an input file.\n");
    }

    if(param.action != 4 && param.out_file == NULL)
    {
        param.action = 0;
        printf("Please specify an output file.\n");
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
    printf("elfcc -d file -o output_file: decompile an elf\n");
    printf("elfcc -c file -o output_file: compile into an elf\n");
}


void compile(struct parameters *p)
{
    char buffer[1024];
    char *elfs_base_path, *machine;
    struct elfs *elfs;
    elfs = (struct elfs*)malloc(sizeof(struct elfs));
    elfs_base_path = (char*) malloc(strlen(p->in_file)+1);
    strcpy(elfs_base_path, p->in_file);
    *strrchr(elfs_base_path, '.') = '\0';
    p->elfs = fopen(p->in_file, "r");
    if(p->elfs == NULL)
    {
        printf("Can't open elfs input file.\n");
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
    if(machine == NULL)
    {
        printf("Machine is not defined in the ELF HEADER section.\n");
        exit(501);
    }
    if(strcmp(machine, "EM_386")==0 || strcmp(machine, "EM_PPC")==0 || strcmp(machine, "EM_ARM")==0)
        compile32(p->elf, elfs);
    else if(strcmp(machine, "EM_PPC64")==0 || strcmp(machine, "EM_IA64")==0 || strcmp(machine, "EM_X86_64")==0 || strcmp(machine, "EM_AARCH64")==0)
        compile64(p->elf, elfs);
    else
    {
        printf("Machine type %s not understood.\n", machine);
        exit(502);
    }
    fclose(p->elfs);
    fclose(p->elf);
    freeElfsStruct(elfs);
}

void compile32(FILE *out, struct elfs *e)
{

}

void compile64(FILE *out, struct elfs *e)
{

}

void decompile(struct parameters *p)
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
        printf("Can't create output elfs file.\n");
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

void decompile32(struct parameters *p)
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
    shdr = (Elf32_Shdr*) calloc(ehdr->e_shnum, sizeof(Elf32_Shdr));
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
    for(i = 0; i<ehdr->e_shnum; i++)
    {
        sections[i] = (void*) malloc(shdr[i].sh_size);
        fseek(p->elf, shdr[i].sh_offset, SEEK_SET);
        if(shdr[i].sh_size != 0 && fread((void*) sections[i], shdr[i].sh_size, 1, p->elf) == 0)
        {
            printf("Elf parsing error: section %u at offset %llu  of size %llu could not be read.\n", i, shdr[i].sh_offset, shdr[i].sh_size);
            exit(304);
        }
        snprintf(buffer, 1024, "%s.%u", p->out_file, i);
        section_out = fopen(buffer, "w");
        if(section_out == NULL)
        {
            printf("Couldn't create file %s to dump section %u.\n", buffer, i);
            exit(305);
        }
        fwrite(sections[i], shdr[i].sh_size, 1, section_out);
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

    
void decompile64(struct parameters *p)
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
    shdr = (Elf64_Shdr*) calloc(ehdr->e_shnum, sizeof(Elf64_Shdr));
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
    for(i = 0; i<ehdr->e_shnum; i++)
    {
        sections[i] = (void*) malloc(shdr[i].sh_size);
        fseek(p->elf, shdr[i].sh_offset, SEEK_SET);
        if(shdr[i].sh_size != 0 && fread((void*) sections[i], shdr[i].sh_size, 1, p->elf) == 0)
        {
            printf("Elf parsing error: section %u at offset %llu  of size %llu could not be read.\n", i, shdr[i].sh_offset, shdr[i].sh_size);
            exit(304);
        }
        snprintf(buffer, 1024, "%s.%u", p->out_file, i);
        section_out = fopen(buffer, "w");
        if(section_out == NULL)
        {
            printf("Couldn't create file %s to dump section %u.\n", buffer, i);
            exit(305);
        }
        fwrite(sections[i], shdr[i].sh_size, 1, section_out);
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
