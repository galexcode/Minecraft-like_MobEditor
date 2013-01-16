#ifndef EDITOR
#define EDITOR

    int editor(char *mainPath, char *pathModel);
    int addCube(Model *model);
    int initCube(Cube *cube);
    void moveCamera(Point3D *pos, double angleX, double angleY, double *zoom);
    void collisionCursorModel(Model *model, int *indexMemberAffected, int *indexFaceAffected);
    double editCube(Model *model, int modeModelisation, int indexMemberAffected, int indexFaceAffected, int selected);
    void renderMenuEditor(Model *model, Button *mainButton, Button *editionButton, Button *fileButton, Button *toolButton, Button *textureButton, int buttonsToRender, Texture *textureText, int *weightLetter, Text *buff, Texture *textureEditor, Face *selectionTexture, Text *textDimensionResized);
    int removeCube(Model *model, int *indexMemberAffected);
    int moveAndResizeTexture(Texture *tex, double *zoom, int xMin, int *differenceEventX, int *differenceEventY);
    int selectAreaTex(Texture *tex, Point2D coordsArea[2], int *selectioning);
    int collisionCursorTexture(Texture *tex);
    int putTextureOnModel(Model *model, int indexMemberAffected, int indexFaceAffected, Point2D areaTexSelected[2]);
    void reverseTexture(Model *model, int axisReversing, int indexMemberAffected, int indexFaceAffected);
    int copyCube(Model *model, int indexMemberSelected, Cube *cubeCopied, Point3D *translationCopied, Point3D *rotationCopied);
    int pasteCube(Model *model, Cube *cubeCopied, Point3D *translationCopied, Point3D *rotationCopied);

#endif
