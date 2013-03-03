#ifndef ANIMS_H
#define ANIMS_H

    int editAnimations(Model *model, char *mainPath, char *pathModel, Texture *textureText, Texture *texButton, double *angleX, double *angleY, double *zoomModel);
    void renderMenuAnimator(Texture *textureText, int *weightLetter, int buttonsToRender, Button *mainButton, Button *fileButton, Button *editionButton, Button *animationButton, int nbAnimationButtonRendered);
    int addAnimation(Model *model, char *animationName);
    int removeAnimation(Model *model, char *animationName);
    int editReversedAnimation(Model *model, char *animationToReverseName);

#endif
