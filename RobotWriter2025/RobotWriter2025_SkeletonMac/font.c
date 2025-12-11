#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "font.h"

/* 
 * Load the character stroke data from the font file.
 * Returns 1 if successful, 0 if there was an error.
 */
int loadStrokesFile(char *filename, CharacterData font[])
{
    FILE *fp;
    int marker;
    int ascii;
    int strokeCount;
    int i;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: could not open font file %s\n", filename);
        return 0;
    }

    /* Initialise the array so we know if any characters are missing */
    for (i = 0; i < 128; i++)
    {
        font[i].asciiCode = i;
        font[i].strokeCount = 0;
        font[i].width = 0;
    }

    /* Read until the end of the file */
    while (fscanf(fp, "%d %d %d", &marker, &ascii, &strokeCount) == 3)
    {
        int j;
        int x, y, p;

        if (marker != 999)
        {
            /* If this line does not mark a new character just skip it */
            continue;
        }

        if (ascii < 0 || ascii > 127)
        {
            /* Ignore invalid ASCII codes */
            /* Read and discard the stroke lines */
            for (j = 0; j < strokeCount; j++)
            {
                fscanf(fp, "%d %d %d", &x, &y, &p);
            }
            continue;
        }

        font[ascii].asciiCode = ascii;
        font[ascii].strokeCount = strokeCount;

        for (j = 0; j < strokeCount; j++)
        {
            if (fscanf(fp, "%d %d %d", &x, &y, &p) != 3)
            {
                /* Error reading stroke data */
                fclose(fp);
                return 0;
            }

            font[ascii].strokes[j].x = x;
            font[ascii].strokes[j].y = y;
            font[ascii].strokes[j].pen = p;

            /* The last X value is used as the character width */
            if (j == strokeCount - 1)
            {
                font[ascii].width = x;
            }
        }
    }

    fclose(fp);
    return 1;
}

/* 
 * Calculate the scaling factor to convert the 18-unit high font
 * into the requested height in mm.
 */
float calculateScalingFactor(float userHeight)
{
    /* Font is designed with a height of 18 units */
    return userHeight / 18.0f;
}

/*
 * Return the width of the next word starting at startIndex.
 * The width is returned in mm (scaled).
 */
float getWordWidth(char text[], int startIndex, CharacterData font[], float scale)
{
    float width = 0.0f;
    int i = startIndex;
    int ascii;

    while (text[i] != '\0' && text[i] != '\n' && text[i] != ' ')
    {
        ascii = (int)text[i];

        if (ascii >= 0 && ascii < 128)
        {
            width = width + (font[ascii].width * scale);
        }

        i = i + 1;
    }

    return width;
}
