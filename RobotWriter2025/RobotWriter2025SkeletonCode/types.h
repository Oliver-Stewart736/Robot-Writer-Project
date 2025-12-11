#ifndef TYPES_H     // Has TYPES_H been defined already?
#define TYPES_H     // If not, define it now

/* Define Stroke Struct */
typedef struct
{
    int x;      // X coord as an int
    int y;      // Y coord as an int
    int pen;    // Pen up = 0, Pen down =1
} Stroke;

/* Define CharacterData Struct */
typedef struct
{
    int asciiCode;         // ASCII code for the character as an int
    int strokeCount;       // Number of strokes as an int
    Stroke strokes[200];   // Array of stroke commands
    int width;             // Character width in font units as an int
} CharacterData;

#endif                     // End the block

