#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#define INVERTED_BIT (1 << 5)

#pragma pack(push,x1)
#pragma pack(1)

typedef struct
{
    unsigned char  IdSize,
                   MapType,
                   ImageType;
    unsigned short PaletteStart,
                   PaletteSize;
    unsigned char  PaletteEntryDepth;
    unsigned short X,
                   Y,
                   Width,
                   Height;
    unsigned char  ColorDepth,
                   Descriptor;
} TGA_HEADER;

#pragma pack(pop,x1)

int WinTGALoad(const char* fileName, char** buffer, int* width, int* height)
{
    FILE* fp;
    TGA_HEADER   Header;

    if (fopen_s(&fp, fileName, "rb") != 0) {
        return FALSE;
    }

    if (fp == NULL) {
        return FALSE;
    }

    fread(&Header, sizeof(TGA_HEADER), 1, fp);

    *width = Header.Width;
    *height = Header.Height;

    if (Header.ColorDepth == 24) {
        RGBTRIPLE* Buffer24;

        Buffer24 = (RGBTRIPLE*)malloc(sizeof(RGBTRIPLE) * (*width) * (*height));

        if (Buffer24)
        {
            int i = 0;
            int x,
                y;

            fread(Buffer24, sizeof(RGBTRIPLE), (*width) * (*height), fp);

            *buffer = (LPSTR)malloc(3 * (*width) * (*height));

            for (y = 0; y < *height; y++)
                for (x = 0; x < *width; x++)
                {
                    int Index = y * (*width) + x;

                    if (!(Header.Descriptor & INVERTED_BIT))
                        Index = ((*height) - 1 - y) * (*width) + x;

                    (*buffer)[(i * 3)] = Buffer24[Index].rgbtRed;
                    (*buffer)[(i * 3) + 1] = Buffer24[Index].rgbtGreen;
                    (*buffer)[(i * 3) + 2] = Buffer24[Index].rgbtBlue;

                    i++;
                }

            fclose(fp);
            free(Buffer24);
            return(TRUE);
        }
    }

    return(FALSE);
}
