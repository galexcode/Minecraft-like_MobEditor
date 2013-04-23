#ifndef ANIMS_H
#define ANIMS_H

    int editAnimations(Model *model, char *mainPath, char *pathModel, Texture *textureText, Texture *texButton, double *angleX, double *angleY, double *zoomModel);
    void renderMenuAnimator(Texture *textureText, int *weightLetter, int buttonsToRender, Button *mainButton, Button *fileButton, Button *editionButton, Button *animationButton, Button *toolButton, int nbAnimationButtonRendered, Text *currentAnimationName, Text *textDimensionResized, Text *textAdvice);
    int addAnimation(Model *model, char *animationName);
    int removeAnimation(Model *model, char *animationName);
    float editMemberAnimation(Model *model, char *animationName, int typeAnimation, int indexMemberAffected, int indexFaceAffected, int *indexMovement);
    void collisionCursorModelAnimator(int *indexFaceAffected, int *indexAreaSelected);
    Point3D defineCubeOrigin(Model *model, int indexMemberAffected, int indexFaceAffected, int indexAreaSelected);
    void attribMemberOrigin(Model *model, Point3D origin, int indexMemberAffected);
    void getBasicValue(Model *model, int indexMemberSelected, int indexCurrentAnimation, int typeAnimation, int axisAnimated);
    int affectAnimationValue(Model *model, int indexAnimation, int indexMovement, int indexMemberAffected, int period);
    int searchMovement(Model *model, int indexAnimation, int indexMemberAffected, int axisAnimated, int typeAnimation);
    int getFreeMovement(Model *model, int indexAnimation);
    void resetTransformationMember(Model *model, int indexCurrentAnimation, int indexMovement);

#endif
