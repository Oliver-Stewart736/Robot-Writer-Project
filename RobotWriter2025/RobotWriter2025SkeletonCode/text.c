#include <stdio.h>
#include <stdlib.h>
#include "text.h"

/*
 * Loads the file into textBuffer.
 * Returns 1 if successful, 0 if the file could not be opened.
 */
int loadTextFile(char *filename, char textBuffer[])
{
    FILE *fp;       // File pointer
    int i;          // Index
    int ch;         // Stores each character as an int

    /* Open font file in read mode */
    fp = fopen(filename, "r");

    /* If file not opened report error */
    if (fp == NULL)
    {
        printf("Error: could not open text file %s\n", filename);
        return 0;
    }

    i = 0;              // Start at index 0
    ch = fgetc(fp);     // Read the first character from file
    
    /* Loop through the text until end of file or max length is reached (with space for terminator) */
    while (ch != EOF && i < (MAX_TEXT_LENGTH - 1))
    {
        textBuffer[i] = (char)ch;   // Store the character into the buffer
        i++;                        // Increase Index
        ch = fgetc(fp);             // Read the next character from file
    }

    textBuffer[i] = '\0';           // Add a terminator at the end

    fclose(fp);                     // Close the file
    return 1;                       // Return 1 if it succeeds
}
