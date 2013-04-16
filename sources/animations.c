#include "constants.h"
#include "animations.h"
#include "input.h"
#include "editor.h"
#include "render.h"
#include "model.h"
#include "button.h"
#include "tools.h"
#include "text.h"
#include "sdlglutils.h"

extern Input event;
extern int weightLetter[256];
extern int windowWidth;
extern int windowHeight;

int editAnimations(Model *model, char *mainPath, char *pathModel, Texture *textureText, Texture *texButton, double *angleX, double *angleY, double *zoomModel)
{
    int i;

    int leave = 0, dataReturn = 1;
    Point3D pos = {2.0, 2.0, 2.0}, target = {0.0, 0.0, 0.0}, origin = {0.0, 0.0, 0.0};
    int indexMemberAffected = -1, indexFaceAffected = -1;
    int indexAreaSelected = -1;
    int FOV = 70;
    int previousTicks = SDL_GetTicks(), actualTicks = SDL_GetTicks();
    int buttonsToRender = FILE_BUTTONS;
    int buttonSelected = 0;

    int modelSelected = 0;

    int typeAnimation = ROTATION_ANIMATION;
    int indexCurrentAnimation = -1;
    int stateSelection = NONE;
    int editingAnimation = 0;
    int indexMemberSelected = -1;
    int indexMovement = -1;
    int definingOrigin = 0;

    char currentEditionAnimation[SIZE_PATH_MAX] = {0};
    char animationPlaying[SIZE_PATH_MAX] = {0};
    char nextAnimation[SIZE_PATH_MAX] = {0};

    Text textCurrentEditionAnimation;
    Text textAdvice;
    Text textDimensionResized;

    float dimensionResized = -1;

    Text buff;

    Button *mainButton;
    Button *fileButton;
    Button *editionButton;
    Button *animationButton;
    Button *toolButton;

    mainButton = malloc(NUMBER_MAIN_BUTTONS_ANIMATOR * sizeof(Button));
    fileButton = malloc(NUMBER_FILE_BUTTONS_ANIMATOR * sizeof(Button));
    editionButton = malloc(NUMBER_EDITION_BUTTONS_ANIMATOR * sizeof(Button));
    toolButton = malloc(NUMBER_TOOL_BUTTONS_ANIMATOR * sizeof(Button));

    if(mainButton == NULL || fileButton == NULL || editionButton == NULL || toolButton == NULL)
    {
        printf("Error allocating button's memory\n");
        return 0;
    }

    attribMainButtonsAnimator(mainButton, texButton);
    attribFileButtonsAnimator(fileButton, texButton);
    attribEditionButtonsAnimator(editionButton, texButton);
    attribToolButtonsAnimator(toolButton, texButton);

    textCurrentEditionAnimation.nbChar = 0;
    textDimensionResized.nbChar = 0;
    textAdvice.nbChar = 0;

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
        animationButton = malloc((model->nbAnims + 2) * sizeof(Button));
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
                    for(i = 0; i < model->nbAnims + 2; i++)
                    {
                        if(buttonCollision(&animationButton[i]))
                            buttonSelected = 1;
                    }
                }
                break;
            case TOOL_BUTTONS:
                for(i = 0; i < NUMBER_TOOL_BUTTONS_ANIMATOR; i++)
                {
                    if(buttonCollision(&toolButton[i]))
                        buttonSelected = 1;
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

        if(mainButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && !modelSelected)
        {
            if(buttonsToRender != FILE_BUTTONS)
                buttonsToRender = FILE_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }
        if(mainButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && !modelSelected)
        {
            if(buttonsToRender != EDITION_BUTTONS)
                buttonsToRender = EDITION_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(mainButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && !modelSelected)
        {
            if(buttonsToRender != ANIMATION_BUTTONS)
                buttonsToRender = ANIMATION_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(mainButton[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && !modelSelected)
        {
            if(buttonsToRender != TOOL_BUTTONS)
                buttonsToRender = TOOL_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        ///Manage File Button Collision

        if(buttonsToRender == FILE_BUTTONS && !modelSelected)
        {
            if(fileButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(openBrowser(buff.string, MODELS))
                {
                    currentEditionAnimation[0] = 0;
                    nextAnimation[0] = 0;
                    createModel(mainPath, buff.string, model);
                    sprintf(pathModel, buff.string);
                    if(model->nbAnims != 0)
                    {
                        free(animationButton);
                        animationButton = malloc((model->nbAnims + 2) * sizeof(Button));
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

        if(buttonsToRender == EDITION_BUTTONS && !modelSelected)
        {
            if(editionButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                addAnimation(model, "New Animation");
                addStringToText(&textAdvice, "Animation created, rename it with a right-clic");

                free(animationButton);
                animationButton = malloc((model->nbAnims + 2) * sizeof(Button));
                if(animationButton == NULL)
                {
                    printf("Error allocating button's memory\n");
                    return 0;
                }
                attribAnimationButtonsAnimatorWithModelsAnimation(model, animationButton, texButton);

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            if(editionButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(strlen(currentEditionAnimation) > 0)
                {
                    removeAnimation(model, currentEditionAnimation);

                    free(animationButton);

                    if(model->nbAnims > 0)
                    {
                        animationButton = malloc((model->nbAnims + 2) * sizeof(Button));
                        if(animationButton == NULL)
                        {
                            printf("Error allocating button's memory\n");
                            return 0;
                        }
                        attribAnimationButtonsAnimatorWithModelsAnimation(model, animationButton, texButton);
                    }
                    else
                    {
                        animationButton = malloc(NUMBER_ANIMATION_BUTTONS_ANIMATOR * sizeof(Button));
                        if(animationButton == NULL)
                        {
                            printf("Error allocating button's memory\n");
                            return 0;
                        }
                        attribAnimationButtonsAnimator(animationButton, texButton);
                    }

                    addStringToText(&textAdvice, "Animation removed");
                }
                else
                {
                    addStringToText(&textAdvice, "Select an animation before remove it");
                }

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            if(editionButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(editingAnimation == 0)
                {
                    if(typeAnimation == ROTATION_ANIMATION)
                    {
                        typeAnimation = TRANSLATION_ANIMATION;
                        addStringToText(&editionButton[2].text, "Animation : Translation");
                        editionButton[2].weight = getWeightString(editionButton[2].text, weightLetter) + 10;
                    }
                    else
                    {
                        typeAnimation = ROTATION_ANIMATION;
                        addStringToText(&editionButton[2].text, "Animation : Rotation");
                        editionButton[2].weight = getWeightString(editionButton[2].text, weightLetter) + 10;
                    }
                    event.mouse[SDL_BUTTON_LEFT] = 0;
                }
                else
                {
                    addStringToText(&textAdvice, "You cannot change the animation type while editing one");
                }
            }

            if(editionButton[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(stateSelection == SELECTED)
                {
                    if(definingOrigin == 1)
                    {
                        definingOrigin = 0;
                        addStringToText(&editionButton[3].text, "Define Origin");
                        editionButton[3].weight = getWeightString(editionButton[3].text, weightLetter) + 10;
                    }
                    else
                    {
                        definingOrigin = 1;
                        addStringToText(&editionButton[3].text, "Defining ...");
                        editionButton[3].weight = getWeightString(editionButton[3].text, weightLetter) + 10;
                    }
                }
                else
                {
                    addStringToText(&textAdvice, "Select a cube before define his origin");
                }

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        ///Manage Animation Button Collision
        if(model->nbAnims != 0 && buttonsToRender == ANIMATION_BUTTONS && !modelSelected)
        {
            if(animationButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && strlen(animationPlaying) > 0)
            {
                stopAnimation(model, animationPlaying);
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            if(animationButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && strlen(currentEditionAnimation) > 0)
            {
                sprintf(animationPlaying, "%s", currentEditionAnimation);
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            for(i = 2; i < model->nbAnims + 2; i++)
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
                        sprintf(currentEditionAnimation, "%s", animationButton[i].text.string);
                    }
                    event.mouse[SDL_BUTTON_LEFT] = 0;
                }
            }

            for(i = 2; i < model->nbAnims + 2; i++)
            {
                if(animationButton[i].selected == 1 && event.mouse[SDL_BUTTON_RIGHT] == 1)
                {
                    animationButton[i].textInput = 1;
                    sprintf(currentEditionAnimation, "%s", animationButton[i].text.string);
                    SDL_EnableUNICODE(1);
                    event.mouse[SDL_BUTTON_RIGHT] = 0;
                }
            }
        }

        ///Manage Tool Button Collision

        if(buttonsToRender == TOOL_BUTTONS && !modelSelected)
        {
            if(toolButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(currentEditionAnimation[0] != 0)
                {
                    if(stateSelection == NONE)
                    {
                        stateSelection = SELECTING;
                        addStringToText(&toolButton[0].text, "Selecting ...");
                        toolButton[0].weight = getWeightString(toolButton[0].text, weightLetter) + 10;
                    }
                    else if(stateSelection == SELECTING)
                    {
                        stateSelection = NONE;
                        addStringToText(&toolButton[0].text, "Select A Cube");
                        toolButton[0].weight = getWeightString(toolButton[0].text, weightLetter) + 10;

                        indexMemberSelected = -1;
                    }
                    else if(stateSelection == SELECTED)
                    {
                        stateSelection = NONE;
                        addStringToText(&toolButton[0].text, "Select A Cube");
                        toolButton[0].weight = getWeightString(toolButton[0].text, weightLetter) + 10;

                        editingAnimation = 0;
                        addStringToText(&toolButton[2].text, "Start Editing Movement");
                        toolButton[2].weight = getWeightString(toolButton[2].text, weightLetter) + 10;
                        indexMemberSelected = -1;
                        indexMovement = -1;

                        definingOrigin = 0;
                        addStringToText(&editionButton[3].text, "Define Origin");
                        editionButton[3].weight = getWeightString(editionButton[3].text, weightLetter) + 10;
                    }
                }
                else
                {
                    addStringToText(&textAdvice, "Select an animation before selecting a cube");
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(toolButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(indexMemberSelected != -1)
                {
                    if(editingAnimation)
                    {
                        if(indexMovement != -1)
                        {
                            affectAnimationValue(model, indexCurrentAnimation, indexMovement, indexMemberSelected);
                            if(model->animation[indexCurrentAnimation]->firstValueEdited[indexMovement] == 1)
                            {
                                addStringToText(&textAdvice, "Define a second value");
                            }
                            if(model->animation[indexCurrentAnimation]->secondValueEdited[indexMovement] == 1)
                            {
                                editingAnimation = 0;
                                addStringToText(&toolButton[2].text, "Start Editing Movement");
                                addStringToText(&textAdvice, "Movement defined");
                                toolButton[2].weight = getWeightString(toolButton[2].text, weightLetter) + 10;

                                if(model->animation[indexCurrentAnimation]->secondValueEdited[indexMovement] == 0)
                                {
                                    resetTransformationMember(model, indexCurrentAnimation, indexMovement);
                                }
                                indexMovement = -1;
                            }
                        }
                    }
                    else
                    {
                        addStringToText(&textAdvice, "You cannot affect a value if you're not editing a movement");
                    }
                }
                else
                {
                    addStringToText(&textAdvice, "Select a cube before affect any value");
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            if(toolButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if(indexMemberSelected != -1)
                {
                    if(!editingAnimation)
                    {
                        editingAnimation = 1;
                        addStringToText(&toolButton[2].text, "Editing ...");
                        addStringToText(&textAdvice, "Define a value for the movement");
                        toolButton[2].weight = getWeightString(toolButton[2].text, weightLetter) + 10;
                    }
                    else if(editingAnimation)
                    {
                        editingAnimation = 0;
                        addStringToText(&toolButton[2].text, "Start Editing Movement");
                        toolButton[2].weight = getWeightString(toolButton[2].text, weightLetter) + 10;
                        if(indexMovement != -1)
                        {
                            if(model->animation[indexCurrentAnimation]->secondValueEdited[indexMovement] == 0)
                            {
                                resetTransformationMember(model, indexCurrentAnimation, indexMovement);
                            }
                        }
                        indexMovement = -1;
                    }
                }
                else
                {
                    addStringToText(&textAdvice, "Select a cube before editing movement");
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
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

        if(event.mouse[SDL_BUTTON_RIGHT] == 1 && !definingOrigin && stateSelection == SELECTED && editingAnimation != 0)
        {
            if(indexMemberSelected != -1 && indexFaceAffected != -1)
            {
                modelSelected = 1;

                if(strlen(currentEditionAnimation) > 0)
                {
                    dimensionResized = editMemberAnimation(model, currentEditionAnimation, typeAnimation, indexMemberSelected, indexFaceAffected, &indexMovement);
                }
            }
        }

        if((event.mouse[SDL_BUTTON_RIGHT] == 1 || event.mouse[SDL_BUTTON_LEFT] == 1) && definingOrigin && indexFaceAffected != -1 && indexMemberSelected != -1 && stateSelection == SELECTED)
        {
            attribMemberOrigin(model, origin, indexMemberSelected);
            definingOrigin = 0;
            addStringToText(&editionButton[3].text, "Define Origin");
            editionButton[3].weight = getWeightString(editionButton[3].text, weightLetter) + 10;
            event.mouse[SDL_BUTTON_RIGHT] = 0;
            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if((event.mouse[SDL_BUTTON_RIGHT] == 1 || event.mouse[SDL_BUTTON_LEFT] == 1) && indexFaceAffected != -1 && indexMemberAffected != -1 && stateSelection == SELECTING)
        {
            indexMemberSelected = indexMemberAffected;
            indexCurrentAnimation = searchAnimation(model, currentEditionAnimation);
            stateSelection = SELECTED;
            addStringToText(&toolButton[0].text, "Stop Selection");
            toolButton[0].weight = getWeightString(toolButton[0].text, weightLetter) + 10;

            event.mouse[SDL_BUTTON_RIGHT] = 0;
            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(event.mouse[SDL_BUTTON_RIGHT] == 0)
        {
            modelSelected = 0;
        }

        actualTicks = SDL_GetTicks();

        if(actualTicks - previousTicks > 15)
        {
            moveCamera(&pos, *angleX, *angleY, zoomModel);

            if(!modelSelected)
            {
                clearScene();
                modeRender(RENDER_3D, &pos, &target, FOV);

                renderModel(model, COLLISION_MODE_EDITOR);
                collisionCursorModelEditor(&indexMemberAffected, &indexFaceAffected);
            }

            if(!modelSelected && stateSelection == SELECTED && indexMemberAffected != -1)
            {
                clearScene();
                modeRender(RENDER_3D, &pos, &target, FOV);

                renderMember(model, indexMemberSelected, COLLISION_MODE_ANIMATOR);
                collisionCursorModelAnimator(&indexFaceAffected, &indexAreaSelected);
                if(definingOrigin == 1 && indexFaceAffected != -1 && indexAreaSelected != -1 && indexMemberSelected != -1 && indexFaceAffected < 6 && indexAreaSelected < PRECISION_COLLISION * PRECISION_COLLISION && indexMemberAffected < model->nbMembers)
                {
                    origin = defineCubeOrigin(model, indexMemberSelected, indexFaceAffected, indexAreaSelected);
                }
            }

            if(indexMemberAffected != -1 || indexFaceAffected != -1)
            {
                model->saved = 0;
                addStringToText(&fileButton[1].text, "Save Model");
            }

            clearScene();
            modeRender(RENDER_3D, &pos, &target, FOV);

            if(stateSelection != SELECTED)
            {
                renderModel(model, RENDER_MODE);
            }
            else
            {
                if(indexMemberSelected != -1)
                {
                    renderMember(model, indexMemberSelected, RENDER_MODE);
                }
            }
            if(definingOrigin)
            {
                glPushMatrix();
                glTranslatef(origin.x, origin.y, origin.z);
                drawAxis(2);
                glPopMatrix();
            }

            if(strlen(animationPlaying) > 0)
            {
                if(animateModel(model, animationPlaying) == 2)
                {
                    if(strlen(nextAnimation) > 0)
                    {
                        sprintf(animationPlaying, "%s", nextAnimation);
                        sprintf(currentEditionAnimation, "%s", nextAnimation);
                        nextAnimation[0] = 0;
                    }
                    else
                    {
                        animationPlaying[0] = 0;
                    }
                }
            }

            modeRender(RENDER_2D, &pos, &target, FOV);

            sprintf(textDimensionResized.string, "%f", dimensionResized);
            textDimensionResized.nbChar = strlen(textDimensionResized.string);

            if(strlen(currentEditionAnimation) > 0)
                sprintf(textCurrentEditionAnimation.string, "%s", currentEditionAnimation);
            else
                sprintf(textCurrentEditionAnimation.string, "No Current Animation");

            textCurrentEditionAnimation.nbChar = strlen(textCurrentEditionAnimation.string);

            if(model->nbAnims == 0)
                renderMenuAnimator(textureText, weightLetter, buttonsToRender, mainButton, fileButton, editionButton, animationButton, toolButton, NUMBER_ANIMATION_BUTTONS_ANIMATOR, &textCurrentEditionAnimation, &textDimensionResized, &textAdvice);
            else
                renderMenuAnimator(textureText, weightLetter, buttonsToRender, mainButton, fileButton, editionButton, animationButton, toolButton, model->nbAnims + 2, &textCurrentEditionAnimation, &textDimensionResized, &textAdvice);

            refreshScene();

            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }

        for(i = 2; i < model->nbAnims + 2; i++)
        {
            if(animationButton[i].textInput == 1)
            {
                addCharToString(&animationButton[i].text, getCharFromKeyboard());
                if(event.keydown[SDLK_RETURN] == 1)
                {
                    animationButton[i].textInput = 0;

                    if(strcmp(model->animation[i - 2]->animationName, animationPlaying) == 0)
                    {
                        sprintf(nextAnimation, "%s", animationButton[i].text.string);
                    }

                    sprintf(currentEditionAnimation, "%s", animationButton[i].text.string);
                    sprintf(model->animation[i - 2]->animationName, "%s", animationButton[i].text.string);

                    SDL_EnableUNICODE(0);
                }
            }
        }
    }

    free(mainButton);
    free(fileButton);
    free(editionButton);
    free(animationButton);
    free(toolButton);

    return dataReturn;
}

void renderMenuAnimator(Texture *textureText, int *weightLetter, int buttonsToRender, Button *mainButton, Button *fileButton, Button *editionButton, Button *animationButton, Button *toolButton, int nbAnimationButtonRendered, Text *currentAnimationName, Text *textDimensionResized, Text *textAdvice)
{
    int i;

    glPushMatrix();
    glTranslated(0, 0, -100);

    for(i = 0; i < NUMBER_MAIN_BUTTONS_ANIMATOR; i++)
    {
        renderButton(&mainButton[i], textureText);
    }

    switch(buttonsToRender)
    {
        case FILE_BUTTONS:
            for(i = 0; i < NUMBER_FILE_BUTTONS_ANIMATOR; i++)
            {
                renderButton(&fileButton[i], textureText);
            }
            break;
        case EDITION_BUTTONS:
            for(i = 0; i < NUMBER_EDITION_BUTTONS_ANIMATOR; i++)
            {
                renderButton(&editionButton[i], textureText);
            }
            break;
        case ANIMATION_BUTTONS:
            for(i = 0; i < nbAnimationButtonRendered; i++)
            {
                renderButton(&animationButton[i], textureText);
            }
            break;
        case TOOL_BUTTONS:
            for(i = 0; i < NUMBER_TOOL_BUTTONS_ANIMATOR; i++)
            {
                renderButton(&toolButton[i], textureText);
            }
            break;
        default:
            break;
    }

    writeText(textureText, *currentAnimationName, weightLetter, windowWidth - getWeightString(*currentAnimationName, weightLetter) - 5, windowHeight - 21);
    writeText(textureText, *textDimensionResized, weightLetter, windowWidth - getWeightString(*textDimensionResized, weightLetter) - 5, windowHeight - 37);
    writeText(textureText, *textAdvice, weightLetter, windowWidth - getWeightString(*textAdvice, weightLetter) - 5, windowHeight - 58);

    glPopMatrix();
}

int addAnimation(Model *model, char *animationName)
{
    int i;

    if(model->nbAnims < ANIMS_MAX)
    {
        model->animation[model->nbAnims] = malloc(sizeof(Animation));

        if(model->animation[model->nbAnims] == NULL)
        {
            printf("Error allocating animation's memory\n");
            return 0;
        }
        model->animation[model->nbAnims]->nbMovements = 0;
        model->animation[model->nbAnims]->lastUpdate = -1;
        model->animation[model->nbAnims]->isReversing = 0;
        sprintf(model->animation[model->nbAnims]->animationName, "%s", animationName);

        for(i = 0; i < MEMBERS_MAX; i++)
        {
            model->animation[model->nbAnims]->basicValueEdited[i] = 0;
            model->animation[model->nbAnims]->firstValueEdited[i] = 0;
            model->animation[model->nbAnims]->secondValueEdited[i] = 0;
            model->animation[model->nbAnims]->initialPhase[i] = INCREASING;
            model->animation[model->nbAnims]->currentPhase[i] = INCREASING;
            model->animation[model->nbAnims]->period[i] = 500;
            model->animation[model->nbAnims]->phaseChanged[i] = 0;
        }

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

float editMemberAnimation(Model *model, char *animationName, int typeAnimation, int indexMemberAffected, int indexFaceAffected, int *indexMovement)
{
    float *dataToEdit = NULL;
    int indexAnimation = -1;
    int axisAnimated = 0;

    if(indexMemberAffected == -1 || indexFaceAffected == -1)
    {
        return -1;
    }

    indexAnimation = searchAnimation(model, animationName);

    if(indexAnimation == -1)
    {
        printf("Animation Not Found : %s\n", animationName);
        return 0;
    }

    if(indexFaceAffected == 0 || indexFaceAffected == 2)
        axisAnimated = X;

    else if(indexFaceAffected == 1 || indexFaceAffected == 3)
        axisAnimated = Z;

    else if(indexFaceAffected == 4 || indexFaceAffected == 5)
        axisAnimated = Y;

    if(typeAnimation == ROTATION_ANIMATION)
    {
        switch(axisAnimated)
        {
            case X:
                dataToEdit = &model->rotation[indexMemberAffected]->x;
                break;
            case Y:
                dataToEdit = &model->rotation[indexMemberAffected]->y;
                break;
            case Z:
                dataToEdit = &model->rotation[indexMemberAffected]->z;
                break;
        }
    }
    else if(typeAnimation == TRANSLATION_ANIMATION)
    {
        switch(axisAnimated)
        {
            case X:
                dataToEdit = &model->translation[indexMemberAffected]->x;
                break;
            case Y:
                dataToEdit = &model->translation[indexMemberAffected]->y;
                break;
            case Z:
                dataToEdit = &model->translation[indexMemberAffected]->z;
                break;
        }
    }

    if((*indexMovement) == -1)
    {
        (*indexMovement) = searchMovement(model, indexAnimation, indexMemberAffected, axisAnimated, typeAnimation);

        if((*indexMovement) != -1)
        {
            model->animation[indexAnimation]->basicValueEdited[(*indexMovement)] = 0;
            model->animation[indexAnimation]->firstValueEdited[(*indexMovement)] = 0;
            model->animation[indexAnimation]->secondValueEdited[(*indexMovement)] = 0;
        }

        if((*indexMovement) == -1)
        {
            (*indexMovement) = getFreeMovement(model, indexAnimation);

            if((*indexMovement) == -1)
            {
                return -1;
            }
            model->animation[indexAnimation]->nbMovements++;
        }
    }

    if(event.mouse[SDL_BUTTON_RIGHT] == 1)
    {
        if(model->animation[indexAnimation]->basicValueEdited[(*indexMovement)] == 0)
        {
            getBasicValue(model, 0, indexAnimation, typeAnimation, axisAnimated);
            model->animation[indexAnimation]->basicValueEdited[(*indexMovement)] = 1;
            model->animation[indexAnimation]->typeAnimation[(*indexMovement)] = typeAnimation;
            model->animation[indexAnimation]->axisAnimated[(*indexMovement)] = axisAnimated;
            model->animation[indexAnimation]->indexMemberAffected[(*indexMovement)] = indexMemberAffected;
        }
        else if(model->animation[indexAnimation]->axisAnimated[(*indexMovement)] == axisAnimated)
        {
            if(typeAnimation == TRANSLATION_ANIMATION)
            {
                if(dataToEdit != &model->translation[indexMemberAffected]->y)
                {
                    (*dataToEdit) += event.xrel * 0.005;
                }
                else
                {
                    (*dataToEdit) -= event.yrel * 0.005;
                }
            }
            if(typeAnimation == ROTATION_ANIMATION)
            {
                (*dataToEdit) += event.xrel;
                if((*dataToEdit) > 180)
                    (*dataToEdit) = -180;
                else if((*dataToEdit) < -180)
                    (*dataToEdit) = 180;
            }
        }
    }

    if(event.mouse[SDL_BUTTON_RIGHT] == 0 && model->animation[indexAnimation]->basicValueEdited[(*indexMovement)] == 1)
    {
        (*dataToEdit) = model->animation[indexAnimation]->basicValue[(*indexMovement)];
    }


    return (*dataToEdit);
}

void collisionCursorModelAnimator(int *indexFaceAffected, int *indexAreaSelected)
{
    GLubyte pixel[3];

    glReadPixels(event.posX, windowHeight - event.posY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    (*indexFaceAffected) = -1;
    (*indexAreaSelected) = -1;

    if(pixel[0] != 0)
    {
        (*indexFaceAffected) = pixel[0] - 10;
        (*indexAreaSelected) = (pixel[1] - 10) * PRECISION_COLLISION + (pixel[2] - 10);
    }
}

Point3D defineCubeOrigin(Model *model, int indexMemberAffected, int indexFaceAffected, int indexAreaSelected)
{
    Point3D origin;
    double width, length;
    int i, j;

    j = indexAreaSelected % PRECISION_COLLISION;
    i = (indexAreaSelected - j) / PRECISION_COLLISION;

    if(model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x == model->member[indexMemberAffected]->face[indexFaceAffected].point[2].x)
    {
        width = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z - model->member[indexMemberAffected]->face[indexFaceAffected].point[2].z;
        length = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y - model->member[indexMemberAffected]->face[indexFaceAffected].point[2].y;

        if(width < 0)
            width *= -1;
        if(length < 0)
            length *= -1;

        origin.x = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x;
        origin.z = (width / (PRECISION_COLLISION - 1)) * i + model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z;
        origin.y = (length / (PRECISION_COLLISION - 1)) * j + model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y;
    }
    else if(model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y == model->member[indexMemberAffected]->face[indexFaceAffected].point[2].y)
    {
        width = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x - model->member[indexMemberAffected]->face[indexFaceAffected].point[2].x;
        length = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z - model->member[indexMemberAffected]->face[indexFaceAffected].point[2].z;

        if(width < 0)
            width *= -1;
        if(length < 0)
            length *= -1;

        origin.y = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y;
        origin.x = (width / (PRECISION_COLLISION - 1)) * i + model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x;
        origin.z = (length / (PRECISION_COLLISION - 1)) * j + model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z;
    }
    else if(model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z == model->member[indexMemberAffected]->face[indexFaceAffected].point[2].z)
    {
        width = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x - model->member[indexMemberAffected]->face[indexFaceAffected].point[2].x;
        length = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y - model->member[indexMemberAffected]->face[indexFaceAffected].point[2].y;

        if(width < 0)
            width *= -1;
        if(length < 0)
            length *= -1;

        origin.z = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z;
        origin.x = (width / (PRECISION_COLLISION - 1)) * i + model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x;
        origin.y = (length / (PRECISION_COLLISION - 1)) * j + model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y;
    }

    origin.x += model->translation[indexMemberAffected]->x;
    origin.y += model->translation[indexMemberAffected]->y;
    origin.z += model->translation[indexMemberAffected]->z;

    return origin;
}

void fixCoordsModel(Model *model)
{
    int h, i, j;
    Point3D cubeDimension;
    Point3D origin;
    Cube tmp;

    origin.x = 0;
    origin.y = 0;
    origin.z = 0;

    for(i = 0; i < model->nbMembers; i++)
    {
        tmp = (*model->member[i]);

        cubeDimension.x = model->member[i]->face[0].point[0].x - model->member[i]->face[2].point[2].x;
        cubeDimension.y = model->member[i]->face[0].point[0].y - model->member[i]->face[2].point[2].y;
        cubeDimension.z = model->member[i]->face[0].point[0].z - model->member[i]->face[2].point[2].z;

        if(cubeDimension.x < 0)
            cubeDimension.x *= -1;
        if(cubeDimension.y < 0)
            cubeDimension.y *= -1;
        if(cubeDimension.z < 0)
            cubeDimension.z *= -1;

        model->translation[i]->x = model->member[i]->face[0].point[0].x + model->translation[i]->x;
        model->translation[i]->y = model->member[i]->face[0].point[0].y + model->translation[i]->y;
        model->translation[i]->z = model->member[i]->face[0].point[0].z + model->translation[i]->z;

        initCube(model->member[i], origin, cubeDimension);

        for(j = 0; j < 6; j++)
        {
            model->member[i]->face[j].color.r = tmp.face[j].color.r;
            model->member[i]->face[j].color.v = tmp.face[j].color.v;
            model->member[i]->face[j].color.b = tmp.face[j].color.b;

            for(h = 0; h < 4; h++)
            {
                model->member[i]->face[j].point[h].coordFileTexture.x = tmp.face[j].point[h].coordFileTexture.x;
                model->member[i]->face[j].point[h].coordFileTexture.y = tmp.face[j].point[h].coordFileTexture.y;
            }
        }
    }
}

void attribMemberOrigin(Model *model, Point3D origin, int indexMemberAffected)
{
    int h, i = indexMemberAffected, j;
    Point3D cubeDimension, cubeZeroPoint;
    Cube tmp;

    tmp = (*model->member[i]);

    cubeDimension.x = model->member[i]->face[0].point[0].x - model->member[i]->face[2].point[2].x;
    cubeDimension.y = model->member[i]->face[0].point[0].y - model->member[i]->face[2].point[2].y;
    cubeDimension.z = model->member[i]->face[0].point[0].z - model->member[i]->face[2].point[2].z;

    if(cubeDimension.x < 0)
        cubeDimension.x *= -1;
    if(cubeDimension.y < 0)
        cubeDimension.y *= -1;
    if(cubeDimension.z < 0)
        cubeDimension.z *= -1;

    cubeZeroPoint.x = model->translation[i]->x + model->member[i]->face[0].point[0].x - origin.x;
    cubeZeroPoint.y = model->translation[i]->y + model->member[i]->face[0].point[0].y - origin.y;
    cubeZeroPoint.z = model->translation[i]->z + model->member[i]->face[0].point[0].z - origin.z;

    model->translation[i]->x += model->member[i]->face[0].point[0].x - cubeZeroPoint.x;
    model->translation[i]->y += model->member[i]->face[0].point[0].y - cubeZeroPoint.y;
    model->translation[i]->z += model->member[i]->face[0].point[0].z - cubeZeroPoint.z;

    initCube(model->member[i], cubeZeroPoint, cubeDimension);

    for(j = 0; j < 6; j++)
    {
        model->member[i]->face[j].color.r = tmp.face[j].color.r;
        model->member[i]->face[j].color.v = tmp.face[j].color.v;
        model->member[i]->face[j].color.b = tmp.face[j].color.b;
        for(h = 0; h < 4; h++)
        {
            model->member[i]->face[j].point[h].coordFileTexture.x = tmp.face[j].point[h].coordFileTexture.x;
            model->member[i]->face[j].point[h].coordFileTexture.y = tmp.face[j].point[h].coordFileTexture.y;
        }
    }
}

void getBasicValue(Model *model, int indexMemberSelected, int indexCurrentAnimation, int typeAnimation, int axisAnimated)
{
    float *value = NULL;

    if(typeAnimation == ROTATION_ANIMATION)
    {
        switch(axisAnimated)
        {
            case X_AXIS:
                value = &model->rotation[indexMemberSelected]->x;
                break;
            case Y_AXIS:
                value = &model->rotation[indexMemberSelected]->y;
                break;
            case Z_AXIS:
                value = &model->rotation[indexMemberSelected]->z;
                break;
        }
    }
    else if(typeAnimation == TRANSLATION_ANIMATION)
    {
        switch(axisAnimated)
        {
            case X_AXIS:
                value = &model->translation[indexMemberSelected]->x;
                break;
            case Y_AXIS:
                value = &model->translation[indexMemberSelected]->y;
                break;
            case Z_AXIS:
                value = &model->translation[indexMemberSelected]->z;
                break;
        }
    }

    model->animation[indexCurrentAnimation]->basicValue[indexMemberSelected] = (*value);
}

int affectAnimationValue(Model *model, int indexAnimation, int indexMovement, int indexMemberAffected)
{
    float *value = NULL;
    float tmp;

    if(model->animation[indexAnimation]->typeAnimation[indexMovement] == ROTATION_ANIMATION)
    {
        switch(model->animation[indexAnimation]->axisAnimated[indexMovement])
        {
            case X_AXIS:
                value = &model->rotation[indexMemberAffected]->x;
                break;
            case Y_AXIS:
                value = &model->rotation[indexMemberAffected]->y;
                break;
            case Z_AXIS:
                value = &model->rotation[indexMemberAffected]->z;
                break;
        }
    }
    else if(model->animation[indexAnimation]->typeAnimation[indexMovement] == TRANSLATION_ANIMATION)
    {
        switch(model->animation[indexAnimation]->axisAnimated[indexMovement])
        {
            case X_AXIS:
                value = &model->translation[indexMemberAffected]->x;
                break;
            case Y_AXIS:
                value = &model->translation[indexMemberAffected]->y;
                break;
            case Z_AXIS:
                value = &model->translation[indexMemberAffected]->z;
                break;
        }
    }
    else
    {
        return 0;
    }

    if(model->animation[indexAnimation]->firstValueEdited[indexMovement] == 1 && model->animation[indexAnimation]->secondValueEdited[indexMovement] == 0)
    {
        model->animation[indexAnimation]->maximalValue[indexMovement] = (*value);

        if(model->animation[indexAnimation]->maximalValue[indexMovement] < model->animation[indexAnimation]->minimalValue[indexMovement])
        {
            tmp = model->animation[indexAnimation]->maximalValue[indexMovement];
            model->animation[indexAnimation]->maximalValue[indexMovement] = model->animation[indexAnimation]->minimalValue[indexMovement];
            model->animation[indexAnimation]->minimalValue[indexMovement] = tmp;
        }
        model->animation[indexAnimation]->secondValueEdited[indexMovement] = 1;
        (*value) = model->animation[indexAnimation]->basicValue[indexMovement];
    }
    else
    {
        model->animation[indexAnimation]->minimalValue[indexMovement] = (*value);
        model->animation[indexAnimation]->firstValueEdited[indexMovement] = 1;
        (*value) = model->animation[indexAnimation]->basicValue[indexMovement];
    }

    return 1;
}

int searchMovement(Model *model, int indexAnimation, int indexMemberAffected, int axisAnimated, int typeAnimation)
{
    int i;

    for(i = 0; i < model->animation[indexAnimation]->nbMovements; i++)
    {
        if(model->animation[indexAnimation]->axisAnimated[i] == axisAnimated &&
            model->animation[indexAnimation]->indexMemberAffected[i] == indexMemberAffected &&
            model->animation[indexAnimation]->typeAnimation[i] == typeAnimation)
        {
            return i;
        }
    }

    return -1;
}

int getFreeMovement(Model *model, int indexAnimation)
{
    int i;

    if(model->animation[indexAnimation]->nbMovements >= MEMBERS_MAX)
    {
        return -1;
    }

    for(i = 0; i < MEMBERS_MAX; i++)
    {
        if(model->animation[indexAnimation]->basicValueEdited[i] == 0)
        {
            return i;
        }
    }

    return -1;
}

void resetTransformationMember(Model *model, int indexCurrentAnimation, int indexMovement)
{
    float basicValue = model->animation[indexCurrentAnimation]->basicValue[indexMovement];

    if(model->animation[indexCurrentAnimation]->basicValueEdited[indexMovement] == 1)
    {
        if(model->animation[indexCurrentAnimation]->typeAnimation[indexMovement] == ROTATION_ANIMATION)
        {
            switch(model->animation[indexCurrentAnimation]->axisAnimated[indexMovement])
            {
                case X_AXIS:
                    model->rotation[model->animation[indexCurrentAnimation]->indexMemberAffected[indexMovement]]->x = basicValue;
                    break;
                case Y_AXIS:
                    model->rotation[model->animation[indexCurrentAnimation]->indexMemberAffected[indexMovement]]->y = basicValue;
                    break;
                case Z_AXIS:
                    model->rotation[model->animation[indexCurrentAnimation]->indexMemberAffected[indexMovement]]->z = basicValue;
                    break;
            }
        }

        else if(model->animation[indexCurrentAnimation]->typeAnimation[indexMovement] == TRANSLATION_ANIMATION)
        {
            switch(model->animation[indexCurrentAnimation]->axisAnimated[indexMovement])
            {
                case X_AXIS:
                    model->translation[model->animation[indexCurrentAnimation]->indexMemberAffected[indexMovement]]->x = basicValue;
                    break;
                case Y_AXIS:
                    model->translation[model->animation[indexCurrentAnimation]->indexMemberAffected[indexMovement]]->y = basicValue;
                    break;
                case Z_AXIS:
                    model->translation[model->animation[indexCurrentAnimation]->indexMemberAffected[indexMovement]]->z = basicValue;
                    break;
            }
        }

        model->animation[indexCurrentAnimation]->basicValueEdited[indexMovement] = 0;
        model->animation[indexCurrentAnimation]->firstValueEdited[indexMovement] = 0;
        model->animation[indexCurrentAnimation]->secondValueEdited[indexMovement] = 0;

        model->animation[indexCurrentAnimation]->nbMovements--;
    }
}
