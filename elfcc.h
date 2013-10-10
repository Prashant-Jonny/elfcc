#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct parameters 
{
    int action;
    char *infile;
    char *outfile;
};

void version();

void usage();

void compile(struct parameters *param);

void decompile(struct parameters *param);

void patch(struct parameters *param);
