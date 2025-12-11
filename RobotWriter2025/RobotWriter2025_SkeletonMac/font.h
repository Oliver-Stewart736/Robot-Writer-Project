#ifndef FONT_H
#define FONT_H

#include "types.h"

int loadStrokesFile(char *filename, CharacterData font[]);
float calculateScalingFactor(float userHeight);
float getWordWidth(char text[], int startIndex, CharacterData font[], float scale);

#endif
