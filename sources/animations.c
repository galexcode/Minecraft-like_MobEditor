#include "constants.h"
#include "animations.h"
#include "input.h"
#include "editor.h"
#include "render.h"
#include "model.h"
#include "button.h"
#include "tools.h"
#include "text.h"

extern Input event;
extern int weightLetter[256];

int editAnimations(Model *model, char *mainPath, char *pathModel, Texture *textureText, Texture *texButton, double *angleX, double *angleY, double *zoomModel)
{
    int i;

    int leave = 0, dataReturn = 1;
    Point3D pos = {2.0, 2.0, 2.0}, target = {0.0, 0.0, 0.0}; //origin = {0.0, 0.0, 0.0};
    int indexMemberAffected = -1, indexFaceAffected = -1;
    int FOV = 70;
    int previousTicks = SDL_GetTicks(), actualTicks = SDL_GetTicks();
    int buttonsToRender = FILE_BUTTONS;
    int buttonSelected = 0;

    //int typeAnimation = ROTATION_ANIMATION;
    char animationPlaying[SIZE_PATH_MAX] = {0};
    char nextAnimation[SIZE_PATH_MAX] = {0};

    Text buff;

    Button *mainButton;
    Button *fileButton;
    Button *editionButton;
    Button *animationButton;

    //editReversedAnimation(model, "laugh");

    mainButton = malloc(NUMBER_MAIN_BUTTONS_ANIMATOR * sizeof(Button));
    fileButton = malloc(NUMBER_FILE_BUTTONS_ANIMATOR * sizeof(Button));
    editionButton = malloc(NUMBER_EDITION_BUTTONS_ANIMATOR * sizeof(Button));

    if(mainButton == NULL || fileButton == NULL || editionButton == NULL)
    {
        printf("Error allocating button's memory\n");
        return 0;
    }

    attribMainButtonsAnimator(mainButton, texButton);
    attribFileButtonsAnimator(fileButton, texButton);
    attribEditionButtonsAnimator(editionButton, texButton);

    if(model->nbAnims == 0)
    {
        animationButton = malloc(NUMBER_ANIMATION_BUTTONS_ANIMATOR * sizeof(Button));
        if(animationButton == NULL)
        {
            printf("Error allocating button's memory\n");
            return 0;
        }
        attribAnimationButtonsAnimator(animationButton, texButton);
    }
    else
    {
        animationButton = malloc((model->nbAnims + 1) * sizeof(Button));
        if(animationButton == NULL)
        {
            printf("Error allocating button's memory\n");
            return 0;
        }
        attribAnimationButtonsAnimatorWithModelsAnimation(model, animationButton, texButton);
    }
    event.mouse[SDL_BUTTON_LEFT] = 0;



    while(!leave)
    {
        updateEvents(&event);

        buttonSelected = 0;

        for(i = 0; i < NUMBER_MAIN_BUTTONS_ANIMATOR; i++)
        {
            if(buttonCollision(&mainButton[i]))
                buttonSelected = 1;
        }
        switch(buttonsToRender)
        {
            case FILE_BUTTONS:
                for(i = 0; i < NUMBER_FILE_BUTTONS_ANIMATOR; i++)
                {
                    if(buttonCollision(&fileButton[i]))
                        buttonSelected = 1;
                }
                break;
            case EDITION_BUTTONS:
                for(i = 0; i < NUMBER_EDITION_BUTTONS_ANIMATOR; i++)
                {
                    if(buttonCollision(&editionButton[i]))
                        buttonSelected = 1;
                }
                break;
            case ANIMATION_BUTTONS:
                if(model->nbAnims == 0)
                {
                    for(i = 0; i < NUMBER_ANIMATION_BUTTONS_ANIMATOR; i++)
                    {
                        if(buttonCollision(&animationButton[i]))
                            buttonSelected = 1;
                    }
                }
                else
                {
                    for(i = 0; i < model->nbAnims + 1; i++)
                    {
                        if(buttonCollision(&animationButton[i]))
                            buttonSelected = 1;
                    }
                }
                break;
            default:
                break;
        }

        if(event.leave == 1 || (event.keydown[SDLK_LALT] == 1 && event.keydown[SDLK_F4] == 1))
        {
            leave = 1;
            dataReturn = 0;
        }

        ///Manage Main Button Collision

        if(mainButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
        {
            if(buttonsToRender != FILE_BUTTONS)
                buttonsToRender = FILE_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }
        if(mainButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
        {
            if(buttonsToRender != EDITION_BUTTONS)
                buttonsToRender = EDITION_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(mainButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
        {
            if(buttonsToRender != ANIMATION_BUTTONS)
                buttonsToRender = ANIMATION_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        ///Manage File Button Collision

        if(buttonsToRender == FILE_BUTTONS)
        {
            if(fileButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(openBrowser(buff.string, MODELS))
                {
                    createModel(mainPath, buff.string, model);
                    sprintf(pathModel, buff.string);
                    if(model->nbAnims != 0)
                    {
                        free(animationButton);
                        animationButton = malloc((model->nbAnims + 1) * sizeof(Button));
                        if(animationButton == NULL)
                        {
                            printf("Error allocating button's memory\n");
                            return 0;
                        }
                        attribAnimationButtonsAnimatorWithModelsAnimation(model, animationButton, texButton);
                    }
                    else
                    {
                        free(animationButton);
                        animationButton = malloc(NUMBER_ANIMATION_BUTTONS_ANIMATOR * sizeof(Button));
                        if(animationButton == NULL)
                        {
                            printf("Error allocating button's memory\n");
                            return 0;
                        }
                        attribAnimationButtonsAnimator(animationButton, texButton);
                    }
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(fileButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(saveModel(pathModel, model))
                {
                    model->saved = 1;
                    addStringToText(&fileButton[1].text, "Saved");
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(fileButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(openBrowser(buff.string, MODELS))
                {
                    sprintf(pathModel, buff.string);
                    if(saveModel(buff.string, model))
                    {
                        model->saved = 1;
                        addStringToText(&fileButton[1].text, "Saved");
                    }
                }

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(fileButton[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                leave = 1;
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        ///Manage Edition Button Collision

        ///Manage Animation Button Collision
        if(model->nbAnims != 0 && buttonsToRender == ANIMATION_BUTTONS)
        {
            if(animationButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && strlen(animationPlaying) > 0)
            {
                stopAnimation(model, animationPlaying);
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            for(i = 1; i < model->nbAnims + 1; i++)
            {
                if(animationButton[i].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
                {
                    if(strlen(animationPlaying) > 0)
                    {
                        sprintf(nextAnimation, "%s", animationButton[i].text.string);
                        stopAnimation(model, animationPlaying);
                    }
                    else
                    {
                        sprintf(animationPlaying, "%s", animationButton[i].text.string);
                    }
                    event.mouse[SDL_BUTTON_LEFT] = 0;
                }
            }
        }

        if(event.mouse[SDL_BUTTON_WHEELDOWN] == 1)
        {
            (*zoomModel) += 0.05;
            if((*zoomModel) > 10)
                (*zoomModel) = 10;
        }
        if(event.mouse[SDL_BUTTON_WHEELUP] == 1)
        {
            (*zoomModel) -= 0.05;
            if((*zoomModel) < 1)
                (*zoomModel) = 1;
        }
        if(event.mouse[SDL_BUTTON_MIDDLE] == 1)
        {
            (*angleX) -= event.yrel * 0.2;
            (*angleY) -= event.xrel * 0.2;

            if((*angleX) > 180)
                (*angleX) = 180;
            else if ((*angleX) < 0.01)
                (*angleX) = 0.01;
            if((*angleY) > 360)
                (*angleY) = 0;
            else if ((*angleY) < -360)
                (*angleY) = 360;
        }

        if(event.mouse[SDL_BUTTON_LEFT] == 1)
        {
            if(buttonSelected == 0)
                buttonsToRender = MAIN_BUTTONS;
        }

        actualTicks = SDL_GetTicks();

        if(actualTicks - previousTicks > 15)
        {
            moveCamera(&pos, *angleX, *angleY, zoomModel);

            clearScene();
            modeRender(RENDER_3D, &pos, &target, FOV);

            renderModel(model, COLLISION_MODE);
            collisionCursorModel(model, &indexMemberAffected, &indexFaceAffected);

            clearScene();
            modeRender(RENDER_3D, &pos, &target, FOV);

            renderModel(model, RENDER_MODE);

            if(strlen(animationPlaying) > 0)
            {
                if(animateModel(model, animationPlaying) == 2)
                {
                    if(strlen(nextAnimation) > 0)
                    {
                        sprintf(animationPlaying, "%s", nextAnimation);
                        nextAnimation[0] = 0;
                    }
                    else
                    {
                        animationPlaying[0] = 0;
                    }
                }
            }

            modeRender(RENDER_2D, &pos, &target, FOV);
            if(model->nbAnims == 0)
                renderMenuAnimator(textureText, weightLetter, buttonsToRender, mainButton, fileButton, editionButton, animationButton, NUMBER_ANIMATION_BUTTONS_ANIMATOR);
            else
                renderMenuAnimator(textureText, weightLetter, buttonsToRender, mainButton, fileButton, editionButton, animationButton, model->nbAnims + 1);

            refreshScene();

            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }
    }

    free(mainButton);
    free(fileButton);
    free(editionButton);
    free(animationButton);

    return dataReturn;
}

void renderMenuAnimator(Texture *textureText, int *weightLetter, int buttonsToRender, Button *mainButton, Button *fileButton, Button *editionButton, Button *animationButton, int nbAnimationButtonRendered)
{
    int i;

    glPushMatrix();
    glTranslated(0, 0, -100);

    for(i = 0; i < NUMBER_MAIN_BUTTONS_ANIMATOR; i++)
    {
        renderButton(&mainButton[i], textureText, NULL);
    }

    switch(buttonsToRender)
    {
        case FILE_BUTTONS:
            for(i = 0; i < NUMBER_FILE_BUTTONS_ANIMATOR; i++)
            {
                renderButton(&fileButton[i], textureText, NULL);
            }
            break;
        case EDITION_BUTTONS:
            for(i = 0; i < NUMBER_EDITION_BUTTONS_ANIMATOR; i++)
            {
                renderButton(&editionButton[i], textureText, NULL);
            }
            break;
        case ANIMATION_BUTTONS:
            for(i = 0; i < nbAnimationButtonRendered; i++)
            {
                renderButton(&animationButton[i], textureText, NULL);
            }
            break;
        default:
            break;
    }

    glPopMatrix();
}

int addAnimation(Model *model, char *animationName)
{
    if(model->nbAnims < ANIMS_MAX)
    {
        model->animation[model->nbAnims] = malloc(sizeof(Animation));

        if(model->animation[model->nbAnims] == NULL)
        {
            printf("Error allocating animation's memory\n");
            return 0;
        }
        model->animation[model->nbAnims]->lastUpdate = -1;
        sprintf(model->animation[model->nbAnims]->animationName, "%s", animationName);

        model->nbAnims++;

        return 1;
    }
    else
    {
        printf("Number of animation max reached\n");
        return 0;
    }
}

int removeAnimation(Model *model, char *animationName)
{
    int i;
    int indexAnimation = -1;

    indexAnimation = searchAnimation(model, animationName);

    if(indexAnimation == -1)
    {
        printf("Animation not found : %s\n", animationName);
        return 0;
    }

    free(model->animation[indexAnimation]);

    for(i = indexAnimation; i < model->nbAnims - 1; i++)
    {
        model->animation[i] = model->animation[i + 1];
    }

    model->nbAnims--;

    return 1;
}
