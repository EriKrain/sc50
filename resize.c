#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n-factor infile outfile\n");
        return 1;
    }
    
    // ensure n-factor is in right diapason
    double fFactor = atof(argv[1]);
    if (fFactor <= 0.0 || fFactor > 100.0)
    {
        fprintf(stderr, "N-factor must be in (0.0, 100.0]\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

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
    
    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // make an array for storing all triples
    RGBTRIPLE imageIn[bi.biWidth][abs(bi.biHeight)];
    
    // iterate over infile's scanlines
    for (int lineIn = 0, biHeight = abs(bi.biHeight); lineIn < biHeight; lineIn++)
    {
        // iterate over pixels in scanline
        for (int pixIn = 0; pixIn < bi.biWidth; pixIn++)
        {
            // temporary storage
            RGBTRIPLE triple;
            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            // put triple in the correspoding place in array
            imageIn[pixIn][lineIn] = triple;
        }

        // skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    
    // update BITMAPHEADERs
    bi.biWidth *= fFactor;
    bi.biHeight *= fFactor;   
    
    // determine padding for outfile
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // calcukate new size of image and file
    bi.biSizeImage = (sizeof(RGBTRIPLE) * bi.biWidth + padding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over the outfile scanlines
    for (int lineOut = 0, biHeight = abs(bi.biHeight); lineOut < biHeight; lineOut++)
    {
        //iterate over the outfile pixels
        for (int pixOut = 0; pixOut < bi.biWidth; pixOut++)
        {
            // temporary storage
            RGBTRIPLE triple;
            //choose from image[][] the pixel we need
            triple = imageIn [(int)(pixOut / fFactor)][(int)(lineOut / fFactor)];
            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }
        
        // add padding
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
