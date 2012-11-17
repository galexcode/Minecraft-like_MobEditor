#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "input.h"

void updateEvents(Input* in)
{
    static SDL_Event event;

    in->mouse[SDL_BUTTON_WHEELUP] = 0;
    in->mouse[SDL_BUTTON_WHEELDOWN] = 0;

    in->xrel = 0;
    in->yrel = 0;

    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_KEYDOWN:
            in->keydown[event.key.keysym.sym] = 1;
            break;
        case SDL_KEYUP:
            in->keydown[event.key.keysym.sym] = 0;
            break;
        case SDL_MOUSEMOTION:
            in->posX = event.motion.x;
            in->posY = event.motion.y;
            in->xrel = event.motion.xrel;
            in->yrel = event.motion.yrel;
            break;
        case SDL_MOUSEBUTTONDOWN:
            in->mouse[event.button.button] = 1;
            break;
        case SDL_MOUSEBUTTONUP:
            in->mouse[event.button.button] = 0;
            break;
        case SDL_QUIT:
            in->leave = 1;
            break;
        default:
            break;
    }

    if(event.type == SDL_MOUSEMOTION)
    {
        event.motion.xrel = 0;
        event.motion.yrel = 0;
    }

    event.type = -1;
}

void initialiserInput(Input *in)
{
    int i;

    in->leave = 0;
    in->posX = 0;
    in->posY = 0;
    in->xrel = 0;
    in->yrel = 0;

    for(i = 0; i < 8; i++)
        in->mouse[i] = 0;

    for(i = 0; i < SDLK_LAST; i++)
    {
        in->keydown[i] = 0;
    }
}
