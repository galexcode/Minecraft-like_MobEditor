#ifndef INPUT
#define INPUT

    typedef struct Input Input;
    struct Input
    {
        char keydown[SDLK_LAST];
        char keydownUnicode[SDLK_LAST];
        int posX,posY;
        int xrel,yrel;
        char mouse[8];
        char leave;
    };

    void updateEvents(Input* in);
    void initialiserInput(Input *in);

#endif
