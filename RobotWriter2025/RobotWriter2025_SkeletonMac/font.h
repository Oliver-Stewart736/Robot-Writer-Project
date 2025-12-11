#ifndef FONT_H      // Has FONT_H been defined already?
#define FONT_H      // If not, define it now

#include "types.h"      // Gives access to CharacterData from tpyes

/* Function to load the SingleStrokeFont.txt file into the CharacterData struct */
int loadStrokesFile(char *filename, CharacterData font[]);

/* Function to calculate the scaling factor using the user height whihc is between 4-10mm */
float calculateScalingFactor(float userHeight);

/* Funtion to calculate the width of a word */
float getWordWidth(char text[], int startIndex, CharacterData font[], float scale);

#endif      // End the block
