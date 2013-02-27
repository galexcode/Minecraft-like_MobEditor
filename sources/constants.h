#ifndef CONSTANTS
#define CONSTANTS

    #include <SDL.h>
    #include <SDL/SDL_image.h>
    #include <GL/glew.h>

    #if defined WIN32
        #include <windows.h>
        #include <tchar.h>
    #endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <time.h>
    #include <string.h>
    #include <errno.h>

    #define MEMBERS_MAX 100
    #define ANIMS_MAX 10
    #define NUMBER_MAIN_BUTTONS_EDITOR 4
    #define NUMBER_EDITION_BUTTONS_EDITOR 3
    #define NUMBER_FILE_BUTTONS_EDITOR 4
    #define NUMBER_TOOL_BUTTONS_EDITOR 3
    #define NUMBER_TEXTURE_BUTTONS_EDITOR 3

    #define NUMBER_MAIN_BUTTONS_ANIMATOR 3
    #define SIZE_PATH_MAX 512

    enum{COLLISION_MODE, RENDER_MODE};
    enum{RENDER_2D, RENDER_3D};
    enum{X, Y, Z};
    enum{FACE_SELECTION, CUBE_SELECTION};
    enum{EVENT_FOR_STRING, EVENT_FOR_EDITOR};
    enum{GET_MODEL_PATH, GET_TEXTURE_PATH};
    enum{X_AXIS, Y_AXIS, Z_AXIS};
    enum{MODELS, TEXTURES};
    enum{MAIN_BUTTONS, FILE_BUTTONS, EDITION_BUTTONS, TOOL_BUTTONS, TEXTURE_BUTTONS};
    enum{SCULPT_MODE, TRANSLATION_MODE};
    enum{CLIC_AND_SLIDE, CLIC_SLIDE_CLIC};
    enum{TRANSLATION_ANIMATION, ROTATION_ANIMATION};
    enum{INCREASING, DECREASING};

    int quitSDL();
    int initOpenGL();
    int initSDL(char *windowTitle);

    ///Structures

    typedef struct Point2D Point2D;
    struct Point2D
    {
        double x, y;
    };

    typedef struct Point3D Point3D;
    struct Point3D
    {
        float x, y, z;
        Point2D coordFileTexture;
    };

    typedef struct Color Color;
    struct Color
    {
        double r, v, b;
    };

    typedef struct Texture Texture;//Same coords than SDL
    struct Texture
    {
        char path[SIZE_PATH_MAX];
        GLuint IDtex;//Id of the texture to apply
        Point2D pos;
        Point2D posTex[2];//beginning point of the texture in the file (in pixels)
        int weight, height;//of the texture to apply, pixels
        int wMax, hMax;//Wheight and height of the file
        Color color;
    };

    typedef struct Face Face;
    struct Face
    {
        Point3D point[4];
        Color color;
    };

    typedef struct Cube Cube;
    struct Cube
    {
        Face face[6];
        GLuint IDVBO;
    };

    typedef struct Text Text;
    struct Text
    {
        char string[SIZE_PATH_MAX];
        int nbChar;
        int shadow;
    };

    typedef struct Button Button;
    struct Button
    {
        GLuint IDtex;
        Point2D pos;
        Point2D textureUnselected[2];
        Point2D textureSelected[2];//textureUnselected and textureSelected are arrays because they contain beginning and ending points
        int weight, height;//Of the button
        int wMax, hMax;//Of the total file
        int selected;//If it is selected
        int textInput;
        Text text;
    };

    typedef struct Animation Animation;
    struct Animation
    {
        char animationName[128];
        int nbMembersAffected;
        int *typeAnimation;
        int *indexMemberAffected;
        int *axisAnimated;
        float *minimalValue;
        float *maximalValue;
        int *phase;
        int *period;//ms
        int lastUpdate;
    };

    typedef struct Model Model;
    struct Model
    {
        int nbMembers;
        int init;
        Cube *member[MEMBERS_MAX];
        Point3D *translation[MEMBERS_MAX];
        Point3D *rotation[MEMBERS_MAX];
        int nbAnims;
        Animation *animation[ANIMS_MAX];
        Texture tex;
        int saved;
    };

#endif
