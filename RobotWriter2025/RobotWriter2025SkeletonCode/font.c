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
    // Variable declarations
    FILE *fp;               // File pointer
    int marker;             // Stores the 999 value for a new character 
    int ascii;              // Stores the ASCII code
    int strokeCount;        // Stores the number of strokes
    int i;                  // Loop counter

    /* Open font file in read mode */
    fp = fopen(filename, "r");

    /* If file not opened report error */
    if (fp == NULL)
    {
        printf("Error: could not open font file %s\n", filename);
        return 0;
    }

    /* Create an entry for each ASCII code */
    for (i = 0; i < 128; i++)
    {
        font[i].asciiCode = i;          // Sets ASCII code from 0 - 127
        font[i].strokeCount = 0;        // Leave as 0 for now
        font[i].width = 0;              // Leave as 0 for now
    }

    /* Read until the end of the file */
    while (fscanf(fp, "%d %d %d", &marker, &ascii, &strokeCount) == 3)
    {
        int j;
        int x, y, p;

        
        /* If this line does not mark a new character skip it */
        if (marker != 999)
        {
            continue;
        }

        /* Ignore invalid ASCII codes */
        if (ascii < 0 || ascii > 127)
        {
            /* Read the stroke lines so they get discarded */
            for (j = 0; j < strokeCount; j++)
            {
                fscanf(fp, "%d %d %d", &x, &y, &p);
            }
            continue;
        }

        /* Store the ASCII code and Stroke Count for the character in the font array*/
        font[ascii].asciiCode = ascii;
        font[ascii].strokeCount = strokeCount;

        /* Loop through all the strokes for this character */
        for (j = 0; j < strokeCount; j++)
        {
            /* Read one stroke line */
            int found = fscanf(fp, "%d %d %d", &x, &y, &p);
            if (found != 3)
            {
                if (found == EOF) // If end reached end of file stop reading the file
                {
                    printf("Reached end of file, before all strokes for character\n");
                    break;
                } 
                else 
                {
                    printf("File format not correct, found line with %d numbers, expecting always 3\n", found);
                    return 0;
                }
                
                fclose(fp);
            }

            /* Store the Strokes Count for the character in the font array */
            font[ascii].strokes[j].x = x;
            font[ascii].strokes[j].y = y;
            font[ascii].strokes[j].pen = p;

            /* The last X value is used as the character width */
            if (j == strokeCount - 1)
            {
                font[ascii].width = x;  // Store the Width for the character in the font array
            }
        }
    }

    fclose(fp);     // Close file after reading into font array
    return 1;       // Return 1 if it succeeds
}

/* 
 * Calculate the scaling factor to convert the 18-unit high font
 * into the requested height in mm.
 */
float calculateScalingFactor(float userHeight)
{
    /* Font is designed with a height of 18 units */
    return userHeight / 18.0f;  // Divide by a float as a double is not needed and float is quicker
}

/*
 * Return the width of the next word starting at startIndex.
 * The width is returned in mm (scaled).
 */
float getWordWidth(char text[], int startIndex, CharacterData font[], float scale)
{
    float width = 0.0f;         // Declare width as a float
    int i = startIndex;         // Start from given index
    int ascii;                  // ASCII value of current character

    /* Loop through the text unitl end of string, newline or space */
    while (text[i] != '\0' && text[i] != '\n' && text[i] != ' ')
    {
        ascii = (int)text[i];       // Store the ASCII code of the character

        if (ascii >= 0 && ascii < 128)          // If ASCII value is valid
        {
            width += font[ascii].width * scale + 2.0f * scale;    // Add the width of character scaled to width as well as character spacing
        }

        i++;           // Move to next character
    }

    return width;
}
