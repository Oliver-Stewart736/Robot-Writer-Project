#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "rs232.h"
#include "serial.h"

// Include the header files 
#include "serial.h"
#include "types.h"
#include "font.h"
#include "text.h"

#define bdrate 115200               /* 115200 baud */
#define MAX_LINE_WIDTH 100.0f       // Max line width 

/* Function to send commands to the robot */
void SendCommands (char *buffer );
/* Function to start a new line */
void newLine(float *currentX, float *currentY);
/* Function to handle spaces */
void handleSpace(float *currentX, float *currentY, float scaleFactor);
/* Function to send a character for the robot to write */
void sendCharacter(char c, float *currentX, float *currentY, CharacterData font[], float scale);

int main()
{

    //char mode[]= {'8','N','1',0};
    char buffer[100];                   // Buffer for G-Code
    char textBuffer[MAX_TEXT_LENGTH];   // Stores loaded text
    char filename[50];                  // Stores user input filename
    CharacterData font[128];            // Array of all ASCII character structs
    float userHeight;                   // Height chosen by user stored as a float
    float scaleFactor;                  // Scale factor used when drawing characters

    // If we cannot open the port then give up immediately
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    /* User input for filename */
    printf("\nEnter the text file to load: "); 
    scanf("%s", filename);

    /* User input for font size */
    printf("Enter a drawing height between 4 and 10 mm: ");
    scanf("%f", &userHeight);

    // If height entered is outside the range report and error
    if (userHeight < 4 || userHeight > 10)
    {
        printf("Error: Height must be between 4 and 10 mm.\n");
        CloseRS232Port();   // Before we exit the program we need to close the COM port
        return 1;           // return 1 if there is error
    }

    // Time to wake up the robot
    printf ("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf (buffer, "\n");
     // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    Sleep(100);

    // This is a special case - we wait  until we see a dollar ($)
    WaitForDollar();
    printf ("\nThe robot is now ready to draw\n");

    /* Move robot to start position */
    sprintf (buffer, "G1 X0 Y0 F1000");
    SendCommands(buffer);
    sprintf (buffer, "M3");
    SendCommands(buffer);

    /* Set the pen up*/
    sprintf (buffer, "S0");
    SendCommands(buffer);

    /* Load the font file */
    if (loadStrokesFile("SingleStrokeFont.txt", font) == 0)
    {
        printf("\nError: Could not load SingleStrokeFont.txt\n");
        CloseRS232Port();   // Before we exit the program we need to close the COM port
        return 1;           // return 1 if there is error
    }

    /* Load text file */
    if (loadTextFile(filename, textBuffer) == 0)
    {
        printf("\nError: Could not load %s\n", filename);
        CloseRS232Port();   // Close connection to robot
        return 1;           // return 1 if there is error
    }

    /* Calculate scaling factor */
    scaleFactor = calculateScalingFactor(userHeight);

    float currentX = 0.0f;      // Track X position as a float
    float currentY = 0.0f;      // Track Y position as a float

    int i = 0;                  // Define index for text buffer

    /* Main drawing loop */
    while (textBuffer[i] != '\0')
    {
        char c = textBuffer[i];                     // Stores current character

        /* Handles newlines */
        if (c == '\n')
        {
            newLine(&currentX, &currentY);
            i++;                                    // Move to next character
            continue;
        }

        /* Handles spaces */
        if (c == ' ')
        {
            handleSpace(&currentX, &currentY, scaleFactor);
            i++;                                    // Move to next character
            continue;
        }

        /* If the upcoming word is too long start a new line */
        float upcomingWordWidth = getWordWidth(textBuffer, i, font, scaleFactor);
        if (currentX + upcomingWordWidth > MAX_LINE_WIDTH)
        {
            newLine(&currentX, &currentY);
        }

        /* Draw character */
        sendCharacter(c, &currentX, &currentY, font, scaleFactor);

        /* Add a small spacing so letters don't collide */
        currentX += (2.0f * scaleFactor);

        i++;       // Move to next character
    }

    /* Finish Drawing */
    sprintf (buffer, "S0");           // Pen up
    SendCommands(buffer);

    sprintf (buffer, "G0 X0 Y0");     // Return to starting position
    SendCommands(buffer);
    
    CloseRS232Port();                   // Before we exit the program we need to close the COM port
    printf("Com port now closed\n");

    return (0);
}

// Send the data to the robot - note in 'PC' mode you need to hit space twice
// as the dummy 'WaitForReply' has a getch() within the function.
void SendCommands (char *buffer )
{
    // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    WaitForReply();
    Sleep(100); // Can omit this when using the writing robot but has minimal effect
    // getch(); // Omit this once basic testing with emulator has taken place
}

/* Move to a new line */
void newLine(float *currentX, float *currentY)
{
    char buffer[100];

    float newX = 0.0f;               // New X to start of new line
    float newY = *currentY - 5.0f;   // New Y down 5 mm
 
    // PAGE OVERFLOW CHECK
    if (newY < -100.0f)
    {
        printf("Error: Page overflow cannot move to a new line.\n");
        return;
    }

    sprintf(buffer, "S0");         // Pen up
    SendCommands(buffer);

    sprintf(buffer, "G0 X%.3f Y%.3f", newX, newY);   // Move robot arm
    SendCommands(buffer);

    *currentX = newX;                // Update X
    *currentY = newY;                // Update Y
}

/* Handle spaces*/
void handleSpace(float *currentX, float *currentY, float scaleFactor)
{
    float newX = *currentX + (5.0f * scaleFactor);   // Calculate new X
    float newY = *currentY;                          // Same Y position
 
    char buffer[100];
 
    sprintf(buffer, "S0");        // Pen up
    SendCommands(buffer);
 
    sprintf(buffer, "G0 X%.3f Y%.3f", newX, newY);  // Move robot arm to new X, Y position
    SendCommands(buffer);
 
    *currentX = newX;               // Update stored X value
}

/* Draw a character */
void sendCharacter(char c, float *currentX, float *currentY, CharacterData font[], float scale)
{
    int ascii;                  // Store ASCII  numebr of character
    CharacterData *ch;          // Pointer to the character stroke data
    int i;                      // Index
    float x, y;                 // Scaled target coords as float
    char buffer[100];

    ascii = (int)c;             // Convert character to ASCII code

    /* Ignore invalid ASCII codes */
    if (ascii < 0 || ascii > 127)
    {
        return;
    }

    ch = &font[ascii];              // Get character's data from font array

    /* If character has no strokes  skip it */
    if (ch->strokeCount == 0)
    {
        *currentX += (3.0f * scale);
        return;
    }

    /* Loop through every stroke of the character */
    for (i = 0; i < ch->strokeCount; i++) 
    {
        x = *currentX + (ch->strokes[i].x * scale);     // Calculate new X
        y = *currentY + (ch->strokes[i].y * scale);     // Calculate new Y

        if (ch->strokes[i].pen == 0)                    // If it is a pen up command
        {
            sprintf(buffer, "S0");                    // Pen Up
            SendCommands(buffer);

            sprintf(buffer, "G0 X%.3f Y%.3f", x, y);  // Move to X,Y without drawing
            SendCommands(buffer);
        }
        else                                            // If it is a pen down command
        {
            sprintf(buffer, "S1000");                 // Pen Down
            SendCommands(buffer);

            sprintf(buffer, "G1 X%.3f Y%.3f", x, y);  // Move to X, Y whilst drawing
            SendCommands(buffer);
        }
    }

    *currentX += (ch->width * scale);                   // Update current X
}
