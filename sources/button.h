#ifndef BUTTON
#define BUTTON

    int initButton(Button *button, Texture *tex);
    int renderButton(Button *button, Texture *textureText, int *weightLetter, Text *buff);
    void buttonCollision(Button *button);
    void attribButtons(Button *button, Texture *texButton);

#endif
