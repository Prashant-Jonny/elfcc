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
                param.infile = argv[i];
        }
        else if(strcmp(argv[i], "-c") == 0)
        {
            param.action = 2;
            i++;
            if(i<argc)
                param.infile = argv[i];
        }
        else if(strcmp(argv[i], "-p") == 0)
        {
            param.action = 3;
            i++;
            if(i<argc)
                param.infile = argv[i];
        }
        else if(strcmp(argv[i], "-v") == 0)
        {
            param.action = 4;
        }
        else if(strcmp(argv[i], "-o") == 0)
        {
            i++;
            if(i<argc)
                param.outfile = argv[i];
        }
    }

    if(param.action != 4 && param.infile == NULL)
    {
        param.action = 0;
        printf("Please specify an input file.\n");
    }

    if(param.action != 4 && param.outfile == NULL)
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
        case 3:
            patch(&param);
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
    printf("ELFCC 0.0\n");
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

void decompile(struct parameters *param)
{
}

void patch(struct parameters *param)
{
    printf("Patching not yet implemented.\n");
}
