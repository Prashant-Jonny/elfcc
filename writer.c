#include "writer.h"

void writeEhdr64(FILE *out, Elf64_Ehdr *ehdr)
{
    fprintf(out, "[ELF HEADER]\n");
    writeMagic(out, ehdr->e_ident);
    writeEType(out, ehdr->e_type);
    writeMachine(out, ehdr->e_machine);
    writeVersion(out, ehdr->e_version);
    fprintf(out, "Entry point: %llx\n\n", ehdr->e_entry);
}

void writeMagic(FILE *out, char *ident)
{
    int i;
    fprintf(out, "Magic: ");
    for(i = 0; i<EI_NIDENT; i++)
        fprintf(out, "%02hhx ", ident[i]);
}

void writeEType(FILE *out, int type)
{
    fprintf(out, "\nType: ");
    switch(type)
    {
        case ET_NONE:
            fprintf(out, "ET_NONE\n");
            break;
        case ET_REL:
            fprintf(out, "ET_REL\n");
            break;
        case ET_EXEC:
            fprintf(out, "ET_EXEC\n");
            break;
        case ET_DYN:
            fprintf(out, "ET_DYN\n");
            break;
        case ET_CORE:
            fprintf(out, "ET_CORE\n");
            break;
        default:
            fprintf(out, "%d", type);
    }
}

void writeMachine(FILE *out, int machine)
{
    fprintf(out, "Machine: ");
    switch(machine)
    {
        case EM_NONE:
            fprintf(out, "EM_NONE\n");
            break;
        case EM_M32:
            fprintf(out, "EM_M32\n");
            break;
        case EM_SPARC:
            fprintf(out, "EM_SPARC\n");
            break;
        case EM_386:
            fprintf(out, "EM_386\n");
            break;
        case EM_68K:
            fprintf(out, "EM_68K\n");
            break;
        case EM_88K:
            fprintf(out, "EM_88K\n");
            break;
        case EM_860:
            fprintf(out, "EM_860\n");
            break;
        case EM_MIPS:
            fprintf(out, "EM_MIPS\n");
            break;
        case EM_PARISC:
            fprintf(out, "EM_PARISC\n");
            break;
        case EM_SPARC32PLUS:
            fprintf(out, "EM_SPARC32PLUS\n");
            break;
        case EM_PPC:
            fprintf(out, "EM_PPC\n");
            break;
        case EM_PPC64:
            fprintf(out, "EM_PPC64\n");
            break;
        case EM_S390:
            fprintf(out, "EM_S390\n");
            break;
        case EM_ARM:
            fprintf(out, "EM_ARM\n");
            break;
        case EM_SH:
            fprintf(out, "EM_SH\n");
            break;
        case EM_SPARCV9:
            fprintf(out, "EM_SPARCV9\n");
            break;
        case EM_IA_64:
            fprintf(out, "EM_IA_64\n");
            break;
        case EM_X86_64:
            fprintf(out, "EM_X86_64\n");
            break;
        case EM_VAX:
            fprintf(out, "EM_VAX\n");
            break;
        default:
            fprintf(out, "%d\n", machine);
    }
}

void writeVersion(FILE *out, int version)
{
    fprintf(out, "Version: ");
    switch(version)
    {
        case EV_CURRENT:
            fprintf(out, "EV_CURRENT\n");
            break;
        case EV_NONE:
            fprintf(out, "EV_NONE\n");
            break;
        default:
            fprintf(out, "%d\n", version);
    }


}

void writePhdr64(FILE *out, Elf64_Phdr *phdr, int count)
{
    int i;
    fprintf(out, "[PROGRAM HEADERS]\n");
    for(i = 0; i<count; i++)
    {
        writePType(out, phdr[i].p_type);
    }
}

void writePType(FILE *out, int type)
{
    fprintf(out, "Type: ");
    switch(type)
    {
        case PT_NULL:
            fprintf(out, "PT_NULL\n");
            break;
        case PT_LOAD:
            fprintf(out, "PT_LOAD\n");
            break;
        case PT_DYNAMIC:
            fprintf(out, "PT_DYNAMIC\n");
            break;
        case PT_INTERP:
            fprintf(out, "PT_INTERP\n");
            break;
        case PT_NOTE:
            fprintf(out, "PT_NOTE\n");
            break;
        case PT_SHLIB:
            fprintf(out, "PT_SHLIB\n");
            break;
        case PT_PHDR:
            fprintf(out, "PT_PHDR\n");
            break;
        case PT_LOPROC:
            fprintf(out, "PT_LOPROC\n");
            break;
        case PT_HIPROC:
            fprintf(out, "PT_HIPROC\n");
            break;
        case PT_GNU_STACK:
            fprintf(out, "PT_GNU_STACK\n");
            break;
        default:
            fprintf(out, "%d\n", type);
    }
}

