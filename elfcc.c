#include "elfcc.h"
#include "writer.h"

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
    printf("ELFCC 0.0\nCopyright Charles Hubain <haxelion@gmail.com> @haxelion\nReleased under GPLv3\n");
}

void usage()
{
    printf("elfcc -d file -o output_file: decompile an elf\n");
    printf("elfcc -c file -o output_file: compile into an elf\n");
    printf("elfcc -p file -o output_file: patch an elf\n");
}


void compile(struct parameters *param)
{
    printf("Compilation not yet implemented.\n");
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
        exit(3);
    }
    p->elfs = fopen(elfs_path, "w");
    if(p->elfs == NULL)
    {
        printf("Can't create output elfs file.\n");
        exit(4);
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
        case EM_M32:
        case EM_SPARC:
        case EM_386:
        case EM_68K:
        case EM_88K:
        case EM_860:
        case EM_MIPS:
        case EM_PARISC:
        case EM_SPARC32PLUS:
        case EM_PPC:
        case EM_S390:
        case EM_ARM:
        case EM_SH:
        case EM_VAX:
            arch = 32;
            break;
        case EM_PPC64:
        case EM_SPARCV9:
        case EM_IA_64:
        case EM_X86_64:
            arch = 64;
            break;
        default:
            printf("Unknown architecture, assuming 32 bits.\n");
    }
    return arch;
}

void decompile32(struct parameters *p)
{
    //ADAPT FROM DECOMPILE64
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
