#ifndef EDITOR
#define EDITOR

    int editor();
    int addCube(Model *model);
    int initCube(Cube *cube);
    void deplacerCamera(Point3D *pos, double angleX, double angleY, double *zoom);
    void collisionCursorModel(Model *model, int *indexMemberAffected, int *indexFaceAffected);
    void resizeCube(Model *model, int indexMemberAffected, int indexFaceAffected);
    void renderMenuEditor(Button *button, Texture *textureText, int *weightLetter);
    int removeCube(Model *model, int *indexMemberAffected);
    int attribCube(Cube *cubeCopy, const Cube *cubeToCopy);

#endif
