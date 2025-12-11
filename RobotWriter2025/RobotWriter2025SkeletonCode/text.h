#ifndef TEXT_H  // Has TEXT_H been defined already?    
#define TEXT_H  // If not, define it now

#define MAX_TEXT_LENGTH 256         // Set max length to reduce memory problems

/* Function to load a .txt file into the textbuffer array */
int loadTextFile(char *filename, char textBuffer[]);

#endif          // End block
