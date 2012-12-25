#include "constants.h"
#include "input.h"
#include "model.h"
#include "editor.h"

Input event;
SDL_Surface *window = NULL;

int initSDL(char *windowTitle);
int deleteExeNameFromPath(char *path);
int quitSDL();
int initOpenGL();

int main(int argc, char *argv[])
{
    char mainPath[SIZE_PATH_MAX] = {0};
    char pathModel[SIZE_PATH_MAX] = {0};
    sprintf(mainPath, "%s", argv[0]);
    deleteExeNameFromPath(mainPath);

    if(!initSDL("Mob Editor"))
    {
        exit(EXIT_FAILURE);
    }

    if(!initOpenGL())
    {
        exit(EXIT_FAILURE);
    }

    errno = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    if(argc == 2)
    {
        sprintf(pathModel, argv[1]);
    }

    editor(mainPath, pathModel);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);


    SDL_Quit();

    return 0;
}


int initSDL(char *windowTitle)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_SetVideoMode(WINDOW_WEIGHT, WINDOW_HEIGHT, 32, SDL_OPENGL | SDL_DOUBLEBUF);

    if(window == NULL)
    {
        printf("Window creation failed\n");
        return 0;
    }

    SDL_WM_SetCaption(windowTitle, NULL);

    return 1;
}

int quitSDL()
{
    SDL_Quit();

    return 1;
}

int initOpenGL()
{
    GLenum initOK;

    initOK = glewInit();

    if(initOK != GLEW_OK)
    {
        printf("GLEW initialization failed : %s\n", glewGetErrorString(initOK));
        return 0;
    }

    return 1;
}

int deleteExeNameFromPath(char *path)
{
    int i;
    char c = 0;

    for(i = strlen(path) - 1; i >= 0 && c != '\\'; i--)
    {
        c = path[i];
        if(c != '\\')
        {
            path[i] = 0;
        }
    }

    return 1;
}
