#ifndef TYPES_H
#define TYPES_H

typedef struct
{
    int x;
    int y;
    int pen;   /* 0 = pen up, 1 = pen down */
} Stroke;

typedef struct
{
    int asciiCode;         /* ASCII code for the character */
    int strokeCount;       /* Number of movements */
    Stroke strokes[200];   /* List of stroke movements */
    int width;             /* Character width in font units */
} CharacterData;

#endif
