#ifndef EDITOR
#define EDITOR

    int editor();
    int addCube(Model *model);
    int initCube(Cube *cube);
    void moveCamera(Point3D *pos, double angleX, double angleY, double *zoom);
    void collisionCursorModel(Model *model, int *indexMemberAffected, int *indexFaceAffected);
    double resizeCube(Model *model, int indexMemberAffected, int indexFaceAffected, int selected);
    void renderMenuEditor(Model *model, Button *button, Texture *textureText, int *weightLetter, Text *buff, Texture *textureEditor, Face *selectionTexture, Text *textDimensionResized);
    int removeCube(Model *model, int *indexMemberAffected);
    int moveAndResizeTexture(Texture *tex, double *zoom, int xMin, int *differenceEventX, int *differenceEventY);
    int selectAreaTex(Texture *tex, Point2D coordsArea[2], int *selectioning);
    int collisionCursorTexture(Texture *tex);
    int putTextureOnModel(Model *model, int indexMemberAffected, int indexFaceAffected, Point2D areaTexSelected[2]);

#endif
