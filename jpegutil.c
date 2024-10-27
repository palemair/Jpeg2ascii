/*
 * =====================================================================================
 *
 *       Filename:  image.c
 *
 *    Description:  utilitaire images 
 *
 *        Version:  1.0
 *        Created:  20/12/2023 21:10:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  patricE (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "jpegutil.h"

int readjpeg (char* filename, struct imgdata* ptimg)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* infile;
    JSAMPARRAY buffer = NULL;   /* Output row buffer */
    unsigned int w, h, c, row_stride;
    size_t counter = 0;

    cinfo.err = jpeg_std_error (&jerr);
    jpeg_create_decompress (&cinfo);

    if ((infile = fopen (filename, "rb")) == NULL)
    {
        perror ("fopen");
        jpeg_destroy_decompress (&cinfo);
        return (EXIT_FAILURE);
    }
    jpeg_stdio_src (&cinfo, infile);
    jpeg_read_header (&cinfo, TRUE);
    jpeg_start_decompress (&cinfo);
    w = cinfo.output_width;
    h = cinfo.output_height;
    c = cinfo.output_components;
    row_stride = w * c;

    buffer =
        (*cinfo.mem->alloc_sarray) ((j_common_ptr) & cinfo, JPOOL_IMAGE, row_stride, 1);

    unsigned char* dest = malloc (row_stride * h * sizeof (unsigned char));

    if (dest == NULL)
    {
        perror ("Image malloc");
        jpeg_destroy_decompress (&cinfo);
        fclose (infile);
        return (EXIT_FAILURE);
    }

    while (cinfo.output_scanline < h)
    {
        jpeg_read_scanlines (&cinfo, buffer, 1);
        /* Assume put_scanline_someplace wants a pointer and sample count. */
        memcpy (dest + counter, buffer[0], row_stride);
        counter += row_stride;
    }

    ptimg->data = dest;
    ptimg->w = w;
    ptimg->h = h;
    ptimg->c = c;

    jpeg_finish_decompress (&cinfo);
    jpeg_destroy_decompress (&cinfo);
    fclose (infile);

    return (EXIT_SUCCESS);
}
