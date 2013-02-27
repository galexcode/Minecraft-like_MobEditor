#ifndef MODEL
#define MODEL

    int initModel(Model *model);
    int loadModel(char *mainPath, char *path, Model *model);
    int saveModel(char *path, Model *model);
    int freeModel(Model *model);
    int renderModel(Model *model, int mode);
    int animateModel(Model *model, char *nameAnimation);
    int createModel(char *mainPath, char *path, Model *model);
    int openTexture(char *mainPath, Model *model);

#endif
