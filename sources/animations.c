#include "constants.h"
#include "animations.h"
#include "input.h"
#include "editor.h"
#include "render.h"
#include "model.h"

extern Input event;

int editAnimations(Model *model)
{
    int leave = 0, dataReturn = 1;
    Point3D pos = {2.0, 2.0, 2.0}, target = {0.0, 0.0, 0.0};
    int FOV = 70;
    double zoomModel = 2.0;
    double angleX = 0.1, angleY = 0.0;
    int previousTicks = 0, actualTicks = 0;

    int typeAnimation = ROTATION_ANIMATION;

    int indexMemberAffected = -1;

    while(!leave)
    {
        updateEvents(&event);

        if(event.leave == 1)
        {
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

        if(actualTicks - previousTicks > 15)
        {
            moveCamera(&pos, angleX, angleY, &zoomModel);

            clearScene();
            modeRender(RENDER_3D, &pos, &target, FOV);

            renderModel(model, RENDER_MODE);

            refreshScene();

            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }
    }

    return dataReturn;
}
