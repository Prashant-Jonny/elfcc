/*
This file is part of Elfcc.

Elfcc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Elfcc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Copyright 2014 Charles Hubain <haxelion@gmail.com>
*/

#include "writer.h"

void writeEhdr64(FILE *out, Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, uint32_t scount)
{
    uint32_t section, i;
    uint64_t offset;
    fprintf(out, "[ELF HEADER]\n");
    writeMagic(out, ehdr->e_ident);
    writeEType(out, ehdr->e_type);
    writeMachine(out, ehdr->e_machine);
    writeVersion(out, ehdr->e_version);
    fprintf(out, "Entry point: %llx\n", ehdr->e_entry);
    writeShstrndx(out, ehdr->e_shstrndx);
    offset = -1;
    for(i = 0; i<scount; i++)
    {
        if(shdr[i].sh_offset <= ehdr->e_phoff && ehdr->e_phoff - shdr[i].sh_offset < offset)
        {
            section = i;
            offset = ehdr->e_phoff - shdr[i].sh_offset;
        }
    }
    fprintf(out, "Program headers offset: Section%x+%llx\n", section, offset);
    offset = -1;
    for(i = 0; i<scount; i++)
    {
        if(shdr[i].sh_offset <= ehdr->e_shoff && ehdr->e_shoff - shdr[i].sh_offset < offset)
        {
            section = i;
            offset = ehdr->e_shoff - shdr[i].sh_offset;
        }
    }
    fprintf(out, "Section headers offset: Section%x+%llx\n", section, offset);
    fprintf(out, "\n");
}

void writeEhdr32(FILE *out, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, uint32_t scount)
{
    uint32_t section, i;
    uint32_t offset;
    fprintf(out, "[ELF HEADER]\n");
    writeMagic(out, ehdr->e_ident);
    writeEType(out, ehdr->e_type);
    writeMachine(out, ehdr->e_machine);
    writeVersion(out, ehdr->e_version);
    fprintf(out, "Entry point: %x\n", ehdr->e_entry);
    writeShstrndx(out, ehdr->e_shstrndx);
    offset = -1;
    for(i = 0; i<scount; i++)
    {
        if(shdr[i].sh_offset <= ehdr->e_phoff && ehdr->e_phoff - shdr[i].sh_offset < offset)
        {
            section = i;
            offset = ehdr->e_phoff - shdr[i].sh_offset;
        }
    }
    fprintf(out, "Program headers offset: Section%x+%x\n", section, offset);
    offset = -1;
    for(i = 0; i<scount; i++)
    {
        if(shdr[i].sh_offset <= ehdr->e_shoff && ehdr->e_shoff - shdr[i].sh_offset < offset)
        {
            section = i;
            offset = ehdr->e_shoff - shdr[i].sh_offset;
        }
    }
    fprintf(out, "Section headers offset: Section%x+%x\n", section, offset);
    fprintf(out, "\n");
}

void writeMagic(FILE *out, char *ident)
{
    int i;
    fprintf(out, "Magic: ");
    for(i = 0; i<EI_NIDENT; i++)
        fprintf(out, "%02hhx ", ident[i]);
}

void writeEType(FILE *out, uint32_t type)
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
        case ET_NUM:
            fprintf(out, "ET_NUM\n");
            break;
        default:
            fprintf(out, "%x", type);
    }
}

void writeMachine(FILE *out, uint32_t machine)
{
    fprintf(out, "Machine: ");
    switch(machine)
    {
        case EM_NONE:
            fprintf(out, "EM_NONE\n");
            break;
        case EM_386:
            fprintf(out, "EM_386\n");
            break;
        case EM_PPC:
            fprintf(out, "EM_PPC\n");
            break;
        case EM_PPC64:
            fprintf(out, "EM_PPC64\n");
            break;
        case EM_ARM:
            fprintf(out, "EM_ARM\n");
            break;
        case EM_IA_64:
            fprintf(out, "EM_IA_64\n");
            break;
        case EM_X86_64:
            fprintf(out, "EM_X86_64\n");
            break;
        case EM_AARCH64:
            fprintf(out, "EM_AARCH64\n");
            break;
        default:
            fprintf(out, "%x\n", machine);
    }
}

void writeVersion(FILE *out, uint32_t version)
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
        case EV_NUM:
            fprintf(out, "EV_NUM\n");
            break;
        default:
            fprintf(out, "%x\n", version);
    }
}

void writeShstrndx(FILE *out, uint32_t index)
{
    fprintf(out, "Section header string index: ");
    switch(index)
    {
        case SHN_UNDEF:
            fprintf(out, "SHN_UNDEF\n");
            break;
        case SHN_ABS:
            fprintf(out, "SHN_ABS\n");
            break;
        case SHN_COMMON:
            fprintf(out, "SHN_COMMON\n");
            break;
        case SHN_XINDEX:
            fprintf(out, "SHN_XINDEX\n");
            break;
        default:
            fprintf(out, "%x\n", index);
            break;
    }
}

void writePhdr64(FILE *out, Elf64_Phdr *phdr, Elf64_Shdr *shdr, uint32_t pcount, uint32_t scount)
{
    uint32_t i, j, start_section, end_section;
    uint64_t start_section_offset, end_section_offset, end_offset;
    for(i = 0; i<pcount; i++)
    {
        fprintf(out, "[PROGRAM HEADER %x]\n", i);
        writePType(out, phdr[i].p_type);
        start_section_offset = -1;
        for(j = 0; j<scount; j++)
        {
            if(shdr[j].sh_offset <= phdr[i].p_offset && phdr[i].p_offset - shdr[j].sh_offset < start_section_offset)
            {
                start_section = j;
                start_section_offset = phdr[i].p_offset - shdr[j].sh_offset;
            }
        }
        fprintf(out, "Start: Section%x+%llx\n", start_section, start_section_offset);
        end_offset = phdr[i].p_offset+phdr[i].p_filesz;
        end_section_offset = -1;
        for(j = 0; j<scount; j++)
        {
            if(shdr[j].sh_offset <= end_offset && end_offset - shdr[j].sh_offset < end_section_offset)
            {
                end_section = j;
                end_section_offset = end_offset - shdr[j].sh_offset;
            }
        }
        fprintf(out, "End: Section%x+%llx\n", end_section, end_section_offset);
        fprintf(out, "Virtual address: %llx\n", phdr[i].p_vaddr);
        fprintf(out, "Physical address: %llx\n", phdr[i].p_paddr);
        fprintf(out, "Align: %llx\n", phdr[i].p_align);
        fprintf(out, "Memory size delta: %+llx\n", phdr[i].p_memsz - phdr[i].p_filesz);
        writePFlags(out, phdr[i].p_flags);
        fprintf(out, "\n");
    }
}

void writePhdr32(FILE *out, Elf32_Phdr *phdr, Elf32_Shdr *shdr, uint32_t pcount, uint32_t scount)
{
    uint32_t i, j, start_section, end_section;
    uint32_t start_section_offset, end_section_offset, end_offset;
    for(i = 0; i<pcount; i++)
    {
        fprintf(out, "[PROGRAM HEADER %x]\n", i);
        writePType(out, phdr[i].p_type);
        start_section_offset = -1;
        for(j = 0; j<scount; j++)
        {
            if(shdr[j].sh_offset <= phdr[i].p_offset && phdr[i].p_offset - shdr[j].sh_offset < start_section_offset)
            {
                start_section = j;
                start_section_offset = phdr[i].p_offset - shdr[j].sh_offset;
            }
        }
        fprintf(out, "Start: Section%x+%x\n", start_section, start_section_offset);
        end_offset = phdr[i].p_offset+phdr[i].p_filesz;
        end_section_offset = -1;
        for(j = 0; j<scount; j++)
        {
            if(shdr[j].sh_offset <= end_offset && end_offset - shdr[j].sh_offset < end_section_offset)
            {
                end_section = j;
                end_section_offset = end_offset - shdr[j].sh_offset;
            }
        }
        fprintf(out, "End: Section%x+%x\n", end_section, end_section_offset);
        fprintf(out, "Virtual address: %x\n", phdr[i].p_vaddr);
        fprintf(out, "Physical address: %x\n", phdr[i].p_paddr);
        fprintf(out, "Align: %x\n", phdr[i].p_align);
        fprintf(out, "Memory size delta: %+x\n", phdr[i].p_memsz - phdr[i].p_filesz);
        writePFlags(out, phdr[i].p_flags);
        fprintf(out, "\n");
    }
}

void writePType(FILE *out, uint32_t type)
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
        case PT_GNU_STACK:
            fprintf(out, "PT_GNU_STACK\n");
            break;
        case PT_TLS:
            fprintf(out, "PT_TLS\n");
            break;
        case PT_NUM:
            fprintf(out, "PT_NUM\n");
            break;
        case PT_GNU_EH_FRAME:
            fprintf(out, "PT_GNU_EH_FRAME\n");
            break;
       case PT_GNU_RELRO:
            fprintf(out, "PT_GNU_RELRO\n");
            break;
         default:
            fprintf(out, "%x\n", type);
    }
}

void writePFlags(FILE *out, uint32_t flags)
{
    fprintf(out, "Flags: ");
    if(flags & PF_R)
        fprintf(out, "R");
    if(flags & PF_W)
        fprintf(out, "W");
    if(flags & PF_X)
        fprintf(out, "X");
    fprintf(out, "\n");
}

void writeShdr64(FILE *out, Elf64_Shdr *shdr, uint32_t scount, char *string_section, uint64_t size)
{
    uint32_t i;
    for(i = 0; i<scount; i++)
    {
        fprintf(out, "[SECTION HEADER %x]\n", i);
        if(shdr[i].sh_name < size)
            fprintf(out, "Name: %s\n", &(string_section[shdr[i].sh_name]));
        else
        {
            printf("Invalid string index for section %x name.\n", i);
            fprintf(out, "Name:\n");
        }
        writeSType(out, shdr[i].sh_type);
        writeSFlags(out, shdr[i].sh_flags);
        fprintf(out, "Address: %llx\n", shdr[i].sh_addr);
        fprintf(out, "Link: %x\n", shdr[i].sh_link);
        fprintf(out, "Info: %x\n", shdr[i].sh_info);
        fprintf(out, "Size: %llx\n", shdr[i].sh_size);
        fprintf(out, "Align: %llx\n", shdr[i].sh_addralign);
        fprintf(out, "Entry size: %llx\n", shdr[i].sh_entsize);
        fprintf(out, "\n");
    }
}

void writeShdr32(FILE *out, Elf32_Shdr *shdr, uint32_t scount, char *string_section, uint32_t size)
{
    uint32_t i;
    for(i = 0; i<scount; i++)
    {
        fprintf(out, "[SECTION HEADER %x]\n", i);
        if(shdr[i].sh_name < size)
            fprintf(out, "Name: %s\n", &(string_section[shdr[i].sh_name]));
        else
        {
            printf("Invalid string index for section %x name.\n", i);
            fprintf(out, "Name:\n");
        }
        writeSType(out, shdr[i].sh_type);
        writeSFlags(out, shdr[i].sh_flags);
        fprintf(out, "Address: %x\n", shdr[i].sh_addr);
        fprintf(out, "Link: %x\n", shdr[i].sh_link);
        fprintf(out, "Info: %x\n", shdr[i].sh_info);
        fprintf(out, "Size: %x\n", shdr[i].sh_size);
        fprintf(out, "Align: %x\n", shdr[i].sh_addralign);
        fprintf(out, "Entry size: %x\n", shdr[i].sh_entsize);
        fprintf(out, "\n");
    }
}

void writeSType(FILE *out, uint32_t type)
{
    fprintf(out, "Type: ");
    switch(type)
    {
        case SHT_NULL:
            fprintf(out, "SHT_NULL\n");
            break;
        case SHT_PROGBITS:
            fprintf(out, "SHT_PROGBITS\n");
            break;
        case SHT_SYMTAB:
            fprintf(out, "SHT_SYMTAB\n");
            break;
        case SHT_STRTAB:
            fprintf(out, "SHT_STRTAB\n");
            break;
        case SHT_RELA:
            fprintf(out, "SHT_RELA\n");
            break;
        case SHT_HASH:
            fprintf(out, "SHT_HASH\n");
            break;
        case SHT_DYNAMIC:
            fprintf(out, "SHT_DYNAMIC\n");
            break;
        case SHT_NOTE:
            fprintf(out, "SHT_NOTE\n");
            break;
        case SHT_NOBITS:
            fprintf(out, "SHT_NOBITS\n");
            break;
        case SHT_REL:
            fprintf(out, "SHT_REL\n");
            break;
        case SHT_SHLIB:
            fprintf(out, "SHT_SHLIB\n");
            break;
        case SHT_DYNSYM:
            fprintf(out, "SHT_DYNSYM\n");
            break;
        case SHT_INIT_ARRAY:
            fprintf(out, "SHT_INIT_ARRAY\n");
            break;
        case SHT_FINI_ARRAY:
            fprintf(out, "SHT_FINI_ARRAY\n");
            break;
        case SHT_PREINIT_ARRAY:
            fprintf(out, "SHT_PREINIT_ARRAY\n");
            break;
        case SHT_GROUP:
            fprintf(out, "SHT_GROUP\n");
            break;
        case SHT_SYMTAB_SHNDX:
            fprintf(out, "SHT_SYMTAB_SHNDX\n");
            break;
        case SHT_GNU_ATTRIBUTES:
            fprintf(out, "SHT_GNU_ATTRIBUTES\n");
            break;
        case SHT_GNU_HASH:
            fprintf(out, "SHT_GNU_HASH\n");
            break;
        case SHT_GNU_LIBLIST:
            fprintf(out, "SHT_GNU_LIBLIST\n");
            break;
        case SHT_GNU_verdef:
            fprintf(out, "SHT_GNU_verdef\n");
            break;
        case SHT_GNU_verneed:
            fprintf(out, "SHT_GNU_verneed\n");
            break;
        case SHT_GNU_versym:
            fprintf(out, "SHT_GNU_versym\n");
            break;
        default:
            fprintf(out, "%x\n", type);
    }
}

void writeSFlags(FILE *out, uint32_t flags)
{
    fprintf(out, "Flags: ");
    if(flags & SHF_ALLOC)
        fprintf(out, "A");
    if(flags & SHF_WRITE)
        fprintf(out, "W");
    if(flags & SHF_EXECINSTR)
        fprintf(out, "E");
    if(flags & SHF_MERGE)
        fprintf(out, "M");
    if(flags & SHF_STRINGS)
        fprintf(out, "S");
    fprintf(out, "\n");
}
