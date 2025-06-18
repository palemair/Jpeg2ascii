/* programme jpeg to ascii */
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ncurses.h>
#include <unistd.h>
#include <term.h>
#include "jpegutil.h"

static char liste[2][10] = {
                             {' ', '.', ',', '-', '~', 'c', 'r', '/', 'X', '#'},
                             {' ', '~', 'x', '/', '*', 'u', '|', '>', '<', 'X'}
                           };

static int g = 0;

void printimg (unsigned h, unsigned w, float t[h][w])
{
    for (unsigned int y = 0; y < h; y++)
    {
        for (unsigned int x = 0; x < w; x++)
        {
            g = (int) (t[y][x] * 10);
            printf ("%c", liste[0][g]);
        }
        usleep(50000);
        printf ("\n");
    }
}

char filename[50];
size_t counter = 0;
unsigned int nblin, nbcol;

int main (int argc, char** argv)
{
    switch (argc)
    {
    case 2:
        {
            strcpy (filename, argv[1]);
            nbcol = 110;
            nblin = 40;
            break;
        }

    case 4:
        {
            strcpy (filename, argv[1]);
            nbcol = atoi (argv[2]);
            nblin = atoi (argv[3]);
            break;
        }

    default:
        {
            strcpy (filename, "/usr/local/src/JPEG/img/House.jpg");
            nbcol = 110;
            nblin = 40;
            break;
        }
    }

    int ret = setupterm (NULL, STDOUT_FILENO, NULL);
    unsigned int colterm;

    if (ret == 0)
    {
        colterm = tigetnum ("cols");
    }
    else
    {
        colterm = 0;
        printf ("pas de longueur de lignes et de colonnes");
    }
    nbcol = colterm;
    /* image data */

    struct imgdata photo;

    if (readjpeg (filename, &photo) != EXIT_SUCCESS)
    {
        perror ("readjpeg");
        exit (EXIT_FAILURE);
    }

    unsigned int h, w, dw, dh, modw, modh;

    h = photo.h;
    w = photo.w;
    float coef = ((float) w) / ((float) h);
    nblin = (0.64 * ((float) nbcol) / coef);

    modw = w % nbcol;
    modh = h % nblin;
    dw = w / nbcol;
    dh = h / nblin;

    float (*pdata)[w] = malloc (sizeof (float[h][w]));

    if (pdata == NULL)
    {
        perror ("malloc pdata");
        exit (EXIT_FAILURE);
    }

    float buffer = 0.0;
    unsigned char* ptimg = photo.data;

    for (unsigned int y = 0; y < h; y++)
    {
        for (unsigned int x = 0; x < w; x++)
        {
            buffer =
                (0.2126 * (*ptimg)) + (0.7152 * (*(ptimg + 1))) +
                (0.0722 * (*(ptimg + 2)));
            buffer /= 255;
            ptimg += 3;
            pdata[y][x] = buffer;
            buffer = 0.0;
        }
    }

    free (photo.data);
    float image[nblin][nbcol];
    memset (image, 0.0, sizeof (float[nblin][nbcol]));

    unsigned int u = 0;
    unsigned int v = 0;
    float moyenne = 0.0;

    for (unsigned int x = 0; x < h - (modh + 1); x += dh)
    {
        for (unsigned y = 0; y < w - (modw + 1); y += dw)
        {
            for (unsigned int s = 0; s < dh; s++)
            {
                for (unsigned int t = 0; t < dw; t++)
                {
                    moyenne += pdata[x + s][y + t];
                }
            }
            image[u][v] = moyenne / (dw * dh);
            moyenne = 0.0;
            v++;
        }
        u++;
        v = 0;
    }
    free (pdata);
    printimg (nblin, nbcol, image);

    return 0;
}
