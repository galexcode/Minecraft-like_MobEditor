#include "constants.h"
#include "animations.h"
#include "input.h"
#include "editor.h"
#include "render.h"
#include "model.h"
#include "button.h"

extern Input event;

int editAnimations(Model *model, Texture *textureText, int *weightLetter, Texture *texButton)
{
    int leave = 0, dataReturn = 1;
    Point3D pos = {2.0, 2.0, 2.0}, target = {0.0, 0.0, 0.0}, origin = {0.0, 0.0, 0.0};
    int indexMemberAffected = -1, indexFaceAffected = -1;
    int FOV = 70;
    double zoomModel = 2.0;
    double angleX = 0.1, angleY = 0.0;
    int previousTicks = 0, actualTicks = 0;

    int typeAnimation = ROTATION_ANIMATION;

    Button *mainButton;

    mainButton = malloc(NUMBER_MAIN_BUTTONS_ANIMATOR * sizeof(Button));

    attribMainButtonsAnimator(mainButton, texButton);

    while(!leave)
    {
        updateEvents(&event);

        if(event.leave == 1 || (event.keydown[SDLK_LALT] == 1 && event.keydown[SDLK_F4] == 1))
        {
            fflush(stdout);
            leave = 1;
            dataReturn = 0;
        }

        if(event.mouse[SDL_BUTTON_WHEELDOWN] == 1)
        {
            zoomModel += 0.05;
            if(zoomModel > 10)
                zoomModel = 10;
        }
        if(event.mouse[SDL_BUTTON_WHEELUP] == 1)
        {
            zoomModel -= 0.05;
            if(zoomModel < 1)
                zoomModel = 1;
        }
        if(event.mouse[SDL_BUTTON_MIDDLE] == 1)
        {
            angleX -= event.yrel * 0.2;
            angleY -= event.xrel * 0.2;

            if(angleX > 180)
                angleX = 180;
            else if (angleX < 0.01)
                angleX = 0.01;
            if(angleY > 360)
                angleY = 0;
            else if (angleY < -360)
                angleY = 360;
        }

        actualTicks = SDL_GetTicks();

        if(actualTicks - previousTicks > 15 && !leave)
        {
            moveCamera(&pos, angleX, angleY, &zoomModel);

            clearScene();
            modeRender(RENDER_3D, &pos, &target, FOV);

            renderModel(model, COLLISION_MODE);
            collisionCursorModel(model, &indexMemberAffected, &indexFaceAffected);

            clearScene();
            modeRender(RENDER_3D, &pos, &target, FOV);

            renderModel(model, RENDER_MODE);

            modeRender(RENDER_2D, &pos, &target, FOV);
            renderMenuAnimator(textureText, weightLetter, mainButton);

            refreshScene();

            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }
    }

    free(mainButton);

    return dataReturn;
}

void renderMenuAnimator(Texture *textureText, int *weightLetter, Button *mainButton)
{
    int i;

    glPushMatrix();
    glTranslated(0, 0, -100);

    for(i = 0; i < NUMBER_MAIN_BUTTONS_ANIMATOR; i++)
    {
        renderButton(&mainButton[i], textureText, weightLetter, NULL);
    }

    glPopMatrix();
}
