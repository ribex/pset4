// decode the secret image by altering red pixels

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

// two command line arguments: name of input file, name of output file

// if args != 2, fprintf to stderr a usage message, return 1

// if the file cannot be opened, notify user and return 2

// if output file cannot be opened, notify user and return 3

// if input is not 24bit uncompressed BMP 4.0, notify user and return 4

// open file

// update header's info for outfile

// read clue's scanline, pixel by pixel

// change pixel's color as necessary

// write verdict's scanline, pixel by pixel

// return 0 upon success

}