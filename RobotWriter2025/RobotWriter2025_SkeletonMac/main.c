#include <stdio.h>
#include <stdlib.h>

#define MAC

#ifdef MAC
#include <unistd.h>
#else
#include <Windows.h>
#include "rs232.h"
#include <conio.h>
#endif

#include "serial.h"
#include "types.h"
#include "font.h"
#include "text.h"

#define bdrate 115200               /* 115200 baud */

void SendCommands (char *buffer );
void newLine(float *currentX, float *currentY); 
void sendCharacter(char c, float *currentX, float *currentY, CharacterData font[], float scale);

int main()
{
    char buffer[100];
    char textBuffer[MAX_TEXT_LENGTH];
    CharacterData font[128];
    float userHeight;
    float scaleFactor;

    // If we cannot open the port then give up immediatly
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    // Time to wake up the robot
    printf ("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf (buffer, "\n");
    // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    #ifdef MAC
    sleep(0.1);
    #else
    Sleep(100);
    #endif


    // This is a special case - we wait  until we see a dollar ($)
    WaitForDollar();

    printf ("\nThe robot is now ready to draw\n");

    //These commands get the robot into 'ready to draw mode' and need to be sent before any writing commands
    sprintf (buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf (buffer, "M3\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);
    
    /* Load the font file */
    if (loadStrokesFile("SingleStrokeFont.txt", font) == 0)
    {
        printf("\nError: Could not load SingleStrokeFont.txt\n");
        CloseRS232Port();
        return 1;
    }

    /* Load text from file */
    if (loadTextFile("test.txt", textBuffer) == 0)
    {
        printf("\nError: Could not load test.txt\n");
        CloseRS232Port();
        return 1;
    }

    /* Set a drawing height manual for now*/
    userHeight = 8.0f;

    /* Compute scaling factor */
    scaleFactor = calculateScalingFactor(userHeight);

    float currentX = 0.0f;
    float currentY = 0.0f;

    int i = 0;
    while (textBuffer[i] != '\0')
    {
        char c = textBuffer[i];

        if (c == '\n')
        {
            newLine(&currentX, &currentY);
            i++;
            continue;
        }

        sendCharacter(c, &currentX, &currentY, font, scaleFactor);

        currentX += (2.0f * scaleFactor);

        i++;
    }

    sprintf (buffer, "S0\n");
    SendCommands(buffer);
    sprintf (buffer, "G0 X0 Y0\n");
    SendCommands(buffer);
    // End of sample g-code
    

    // Before we exit the program we need to close the COM port
    CloseRS232Port();
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
    #ifdef MAC
    sleep(0.1);
    #else
    Sleep(100); // Can omit this when using the writing robot but has minimal effect
    #endif
}

void newLine(float *currentX, float *currentY)
{
    *currentX = 0.0f;
    *currentY = *currentY - 5.0f;
}

void sendCharacter(char c, float *currentX, float *currentY, CharacterData font[], float scale)
{
    int ascii;
    CharacterData *ch;
    int i;
    float x, y;
    char buffer[100];

    ascii = (int)c;

    if (ascii < 0 || ascii > 127)
    {
        return;
    }

    ch = &font[ascii];

    if (ch->strokeCount == 0)
    {
        *currentX = *currentX + (3.0f * scale);
        return;
    }

    for (i = 0; i < ch->strokeCount; i++) 
    {
        x = *currentX + (ch->strokes[i].x * scale); 
        y = *currentY + (ch->strokes[i].y * scale);

        if (ch->strokes[i].pen == 0)
        {
            sprintf(buffer, "S0\n");
            SendCommands(buffer);

            sprintf(buffer, "G0 X%.3f Y%.3f\n", x, y);
            SendCommands(buffer);
        }
        else
        {
            sprintf(buffer, "S1000\n");
            SendCommands(buffer);

            sprintf(buffer, "G1 X%.3f Y%.3f\n", x, y);
            SendCommands(buffer);
        }
    }

    *currentX = *currentX + (ch->width * scale);
}