#ifndef MODEL
#define MODEL

    int initModel(Model *model);
    int loadModel(char *path, Model *model);
    int saveModel(char *path, Model *model);
    int freeModel(Model *model);
    int renderModel(Model *model, int mode);

#endif
