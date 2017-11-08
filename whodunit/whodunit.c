// decode the secret image by altering red pixels

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{

    // two command line arguments: name of input file, name of output file
    // ensure proper usage
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./whodunit infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];
    char *outfile = argv[2];

    // open input file
    FILE *inptr = fopen(infile, "r");
    // if the file cannot be opened, notify user and return 2
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    // if output file cannot be opened, notify user and return 3
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // explanation and usage of fread:
    // fread(data, size, number, inptr);
    // data: pointer to a struct that will contain the bytes being read
    // size: size of each element to read (see sizeof)
    // number: number of elements to read
    // inptr: FILE * to read from

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // update header's info for outfile
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // read clue's scanline, pixel by pixel
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // change pixel's color as necessary
            // make a green pixel
            // RGBTRIPLE triple;
            // triple.rgbtBlue = 0x00;
            // triple.rgbtGreen = 0xff;
            // triple.rgbtRed = 0x00;

            // change only pure red pixels to white (0000ff -> ffffff)
            if (triple.rgbtBlue == 0x00 && triple.rgbtGreen == 0x00 && triple.rgbtRed == 0xff)
            {
                triple.rgbtBlue = 0xff;
                triple.rgbtGreen = 0xff;
            }
            // change all white pixels to black
            if (triple.rgbtRed == 0xff && triple.rgbtGreen == 0xff && triple.rgbtBlue == 0xff)
            {
                triple.rgbtBlue = 0x00;
                triple.rgbtGreen = 0x00;
                triple.rgbtRed = 0x00;
            }

            // write verdict's scanline, pixel by pixel
            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // file position indicator
        // fseek(inptr, offset, from);
        // inptr: FILE * to seek over
        // offset: number of bytes to move cursor
        // from: SEEK_CUR (current position in file)
        //      SEEK_SET (beginning of file)
        //      SEEK_END (end of file)
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}



// if (pixels % 4 == 0), no padding
// if (pixels % 4 == 3), 1 padding
// if (pixels % 4 == 2), 2 padding
// if (pixels % 4 == 1), 3 padding