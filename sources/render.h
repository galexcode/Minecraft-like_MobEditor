#ifndef RENDER
#define RENDER

    void lookAt(Point3D *position, Point3D *target);
    void drawCube(Cube *cube);
    void drawFace(Face *face, int mode);
    void clearScene();
    void modeRender(int modeScene, Point3D *pos, Point3D *target, int FOV);
    void refreshScene();
    void drawTexture(Texture *tex);

#endif
