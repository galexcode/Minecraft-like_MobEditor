#ifndef CONSTANTS
#define CONSTANTS

    #include <SDL.h>
    #include <SDL/SDL_image.h>
    #include <GL/glew.h>

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <time.h>
    #include <string.h>
    #include <errno.h>

    #define WINDOW_WEIGHT 1200
    #define WINDOW_HEIGHT 650
    #define MEMBERS_MAX 100
    #define NUMBER_BUTTONS_EDITOR 6
    #define SIZE_PATH_MAX 256

    enum{COLLISION_MODE, RENDER_MODE};
    enum{RENDER_2D, RENDER_3D};
    enum{X, Y, Z};
    enum{FACE_SELECTION, CUBE_SELECTION};
    enum{EVENT_FOR_STRING, EVENT_FOR_EDITOR};
    enum{GET_MODEL_PATH, GET_TEXTURE_PATH};
    enum{X_AXIS, Y_AXIS, Z_AXIS};

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

    typedef struct Model Model;
    struct Model
    {
        int nbMembers;
        int init;
        Cube *member;
        Point3D *translation;
        Texture tex;
        int saved;
    };

#endif
