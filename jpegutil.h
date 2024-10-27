#ifndef JPEGUTIL
#define JPEGUTIL
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jpeglib.h>
#include <setjmp.h>

struct imgdata
{
    unsigned char* data;
    unsigned int w;
    unsigned int h;
    unsigned char c;
};

int readjpeg (char* filename, struct imgdata* ptimg);

#endif
