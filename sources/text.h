#ifndef TEXT_H
#define TEXT_H

    void loadWeightLetters(char *path, int *weightLetter);
    void writeText(Texture *texture, Text text, int *weightLetter, int x, int y);
    int getWeightString(Text text, int weightString[255]);
    int addStringToText(Text *text, char *string);
    int addCharToString(Text *text, int c);
    char getCharFromKeyboard();

#endif
