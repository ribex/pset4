#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cs50.h>

typedef uint8_t  BYTE;

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover filename\n");
        return 1;
    }
    BYTE buffer[512];

    char *infile = argv[1];

    // open memory card file
    FILE *inptr = fopen(infile, "r");

    if (inptr == NULL)
    {
        fprintf(stderr, "The file %s cannot be opened for reading.\n", infile);
        return 2;
    }

    int filenumber = 0;

    FILE *image = NULL;
    bool found = false;

    char file[8];
    char *filename = file;

    while (fread(&buffer, sizeof(buffer), 1, inptr) == 1)
    {
        if (feof(inptr))
        {
            printf("end of file\n");
            return 3;
        }

        // if at the start of a new jpg
        if ((buffer[0] == 0xff) && (buffer[1] == 0xd8) && (buffer[2] == 0xff) && ((buffer[3] & 0xf0) == 0xe0))
        {
            // printf("Found a jpg!\n");
            found = true;

            // if we haven't found a jpg yet
            if (image == NULL)
            {
                // start first jpg
                // open a new jpg file to write to
                // ###.jpg starting 000
                sprintf(filename, "%.3d.jpg", filenumber);
                image = fopen(filename, "a");
                fwrite(&buffer, sizeof(buffer), 1, image);
                filenumber++;
            }

            // if we did find a new jpg then close the previous file
            else
            {
                fclose(image);
                sprintf(filename, "%.3d.jpg", filenumber);
                image = fopen(filename, "w");
                fwrite(&buffer, sizeof(buffer), 1, image);
                filenumber++;
            }
        }
        // if not at the start of a new jpg
        else
        {
            // have we already found a jpg (mid-image)
            if (found == true)
            {
                // write buffer to file
                fwrite(&buffer, sizeof(buffer), 1, image);
            }
            // discard and go to start of loop
            // else
            // {
            //     printf("Not in a jpg yet\n");
            // }
        }
    }

    if ((fread(&buffer, sizeof(buffer), 1, inptr)) == 0)
    {
        return 0;
    }

    // close input file
    fclose(inptr);
    return 0;
}