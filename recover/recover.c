#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bmp.h"
#include <cs50.h>

typedef struct {
    BYTE bytearray[512];
}
BUFFER;

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover filename\n");
        return 1;
    }

    char *infile = argv[1];

    // printf("infile: %s\n", infile);

    // open memory card file
    FILE *inptr = fopen(infile, "r");

    if (inptr == NULL)
    {
        fprintf(stderr, "The file %s cannot be opened for reading.\n", infile);
        return 2;
    }

    int filenumber = 0;

    BUFFER buffer;

    FILE *image = NULL;
    bool found = false;

    char file[8];
    char *filename = file;

    while (fread(&buffer, sizeof(BYTE), 1, inptr) == 1)
    {
        if (feof(inptr))
        {
            printf("end of file\n");
            return 3;
        }

        // if at the start of a new jpg
        if ((buffer.bytearray[0] == 0xff) && (buffer.bytearray[1] == 0xd8) && (buffer.bytearray[2] == 0xff) && ((buffer.bytearray[3] >= 0xe0) && (buffer.bytearray[3] <= 0xef)))
        {
            printf("Found a jpg!\n");
            found = true;

            // if we haven't found a jpg yet
            if (image == NULL)
            {
                // start first jpg
                // open a new jpg file to write to
                // ###.jpg starting 000

                sprintf(filename, "%.3d.jpg", filenumber);
                image = fopen(filename, "a");
                fwrite(&buffer, sizeof(BUFFER), 1, image);
                filenumber++;
            }

            // if we did find a new jpg then close the previous file
            else
            {
                fclose(image);
                sprintf(filename, "%.3d.jpg", filenumber);
                image = fopen(filename, "w");
                fwrite(&buffer, sizeof(BUFFER), 1, image);
                filenumber++;
            }
            // open a new jpg file to write to
            // ###.jpg starting (higher than 000)
        }
        // if not at the start of a new jpg
        else
        {
            // have we already found a jpg (mid-image)
            if (found == true)
            {
                // write buffer to file
                fwrite(&buffer, sizeof(BUFFER), 1, image);
            }
            // else
                // discard and go to start of loop
            else
            {
                printf("Not in a jpg yet\n");
            }
        }



        // char *filename = "";
        // sprintf(filename, "%03i.jpg", filenumber);
        // FILE *img = fopen(filename, "w");



        // fclose(image);
    }

    if ((fread(&buffer, sizeof(BUFFER), 1, inptr)) == 0)
    {
        return 0;
    }

    // detect end of file (block < 512 bytes)

    // close input file
    fclose(inptr);
    return 0;
}