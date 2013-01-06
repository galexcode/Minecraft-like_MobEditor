#ifndef BUTTON
#define BUTTON

    int initButton(Button *button, Texture *tex);
    int renderButton(Button *button, Texture *textureText, int *weightLetter, Text *buff);
    void buttonCollision(Button *button);
    void attribMainButtons(Button *button, Texture *texButton);
    void attribEditionButtons(Button *button, Texture *texButton);
    void attribFileButtons(Button *button, Texture *texButton);
    void attribToolButtons(Button *button, Texture *texButton);
    void attribTextureButtons(Button *button, Texture *texButton);

#endif
