#ifndef BUTTON
#define BUTTON

    int initButton(Button *button, Texture *tex);
    int renderButton(Button *button, Texture *textureText);
    int buttonCollision(Button *button);
    void attribMainButtonsEditor(Button *button, Texture *texButton);
    void attribEditionButtonsEditor(Button *button, Texture *texButton);
    void attribFileButtonsEditor(Button *button, Texture *texButton);
    void attribToolButtonsEditor(Button *button, Texture *texButton);
    void attribTextureButtonsEditor(Button *button, Texture *texButton);

    void attribMainButtonsAnimator(Button *button, Texture *texButton);
    void attribEditionButtonsAnimator(Button *button, Texture *texButton);
    void attribFileButtonsAnimator(Button *button, Texture *texButton);
    void attribAnimationButtonsAnimator(Button *button, Texture *texButton);
    int attribAnimationButtonsAnimatorWithModelsAnimation(Model *model, Button *button, Texture *texButton);

#endif
