#include <stdio.h>
#include <stdlib.h>
#include "text.h"

/*
 * Load a single line of text from the file into textBuffer.
 * Returns 1 if successful, 0 if the file could not be opened.
 */
int loadTextFile(char *filename, char textBuffer[])
{
    FILE *fp;
    int i;
    int ch;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: could not open text file %s\n", filename);
        return 0;
    }

    i = 0;
    ch = fgetc(fp);
    while (ch != EOF && ch != '\n' && i < (MAX_TEXT_LENGTH - 1))
    {
        textBuffer[i] = (char)ch;
        i = i + 1;
        ch = fgetc(fp);
    }

    textBuffer[i] = '\0';

    fclose(fp);
    return 1;
}
