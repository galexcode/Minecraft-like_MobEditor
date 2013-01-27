#include "constants.h"
#include "editor.h"
#include "model.h"
#include "render.h"
#include "input.h"
#include "sdlglutils.h"
#include "button.h"
#include "text.h"
#include "tools.h"
#include "animations.h"

extern Input event;
extern int windowWidth;
extern int windowHeight;

int editor(char *mainPath, char *pathModel)
{
    int i;
    int leave = 0;
    int FOV = 70;
    int previousTicks = 0, actualTicks = 0;
    double angleX = 0.01, angleY = 0, zoomModel = 2, zoomTexEditor = 1;
    int indexMemberAffected = -1, indexFaceAffected = -1;
    int differenceEventX = -1, differenceEventY = -1;
    Face selectionTexture;
    Point2D areaTexSelected[2];
    int selectioningTex = 0;
    Text buff;
    int selected = 0;
    int buttonSelected = 0;
    int weightLetter[256];
    int selection = FACE_SELECTION;
    int stateEvent = EVENT_FOR_EDITOR;
    int axisReversing = X_AXIS;
    int modeModelisation = SCULPT_MODE;
    int buttonsToRender = MAIN_BUTTONS;
    int modeSculpting = CLIC_AND_SLIDE;

    int copied = 0;
    Cube cubeCopied;
    Point3D translationCopied;
    Point3D rotationCopied;

    double dimensionResized = -1;
    double oldDimensionResized = -1;
    Text textDimensionResized;

    Point3D pos, target;
    Model model;

    Button *mainButton;
    Button *fileButton;
    Button *editionButton;
    Button *toolButton;
    Button *textureButton;

    Texture texButton;
    Texture texText;
    Texture textureEditor;

    buff.shadow = 1;

    mainButton = malloc(NUMBER_MAIN_BUTTONS * sizeof(Button));
    editionButton = malloc(NUMBER_EDITION_BUTTONS * sizeof(Button));
    fileButton = malloc(NUMBER_FILE_BUTTONS * sizeof(Button));
    toolButton = malloc(NUMBER_TOOL_BUTTONS * sizeof(Button));
    textureButton = malloc(NUMBER_TEXTURE_BUTTONS * sizeof(Button));

    if(mainButton == NULL || editionButton == NULL || fileButton == NULL || toolButton == NULL || textureButton == NULL)
    {
        printf("Error allocating button's memory\n");
    }

    sprintf(buff.string, "%stextures/font/default.properties", mainPath);
    loadWeightLetters(buff.string, &weightLetter[0]);

    sprintf(texText.path, "%stextures/font/default.png", mainPath);
    loadTexture(&texText);

    sprintf(texButton.path, "%stextures/gui/gui.png", mainPath);
    loadTexture(&texButton);

    sprintf(textureEditor.path, "%stextures/texEditor.png", mainPath);
    loadTexture(&textureEditor);

    textureEditor.color.r = 255;
    textureEditor.color.v = 255;
    textureEditor.color.b = 255;
    textureEditor.height = windowHeight;
    textureEditor.weight = textureEditor.wMax;
    textureEditor.pos.x = windowWidth - textureEditor.weight - 1;
    textureEditor.pos.y = 0;

    attribMainButtons(mainButton, &texButton);
    attribEditionButtons(editionButton, &texButton);
    attribFileButtons(fileButton, &texButton);
    attribToolButtons(toolButton, &texButton);
    attribTextureButtons(textureButton, &texButton);

    selectionTexture.point[0].z = 0;
    selectionTexture.point[1].z = 0;

    areaTexSelected[0].x = -1;
    areaTexSelected[0].y = -1;
    areaTexSelected[1].x = -1;
    areaTexSelected[1].y = -1;

    initModel(&model);

    if(pathModel[0] != 0)
    {
        loadModel(mainPath, pathModel, &model);
    }

    target.x = 0;
    target.y = 0;
    target.z = 0;

    pos.x = 2;
    pos.y = 2;
    pos.z = 2;

    while(!leave)
    {
        buttonSelected = 0;

        updateEvents(&event);

        if(event.leave == 1)
        {
            leave = 1;
            event.leave = 0;
        }
        if(event.keydown[SDLK_LALT] == 1 && event.keydown[SDLK_F4] == 1)
        {
            leave = 1;
            event.keydown[SDLK_LALT] = 0;
            event.keydown[SDLK_ESCAPE] = 0;
        }

        for(i = 0; i < NUMBER_MAIN_BUTTONS; i++)
        {
            buttonCollision(&mainButton[i]);
            buttonSelected = 1;
        }

        if(buttonsToRender == FILE_BUTTONS)
        {
            for(i = 0; i < NUMBER_FILE_BUTTONS; i++)
            {
                buttonCollision(&fileButton[i]);
                buttonSelected = 1;
            }
        }

        if(buttonsToRender == EDITION_BUTTONS)
        {
            for(i = 0; i < NUMBER_EDITION_BUTTONS; i++)
            {
                buttonCollision(&editionButton[i]);
                buttonSelected = 1;
            }
        }

        if(buttonsToRender == TOOL_BUTTONS)
        {
            for(i = 0; i < NUMBER_TOOL_BUTTONS; i++)
            {
                buttonCollision(&toolButton[i]);
                buttonSelected = 1;
            }
        }

        if(buttonsToRender == TEXTURE_BUTTONS)
        {
            for(i = 0; i < NUMBER_TEXTURE_BUTTONS; i++)
            {
                buttonCollision(&textureButton[i]);
                buttonSelected = 1;
            }
        }

        if(event.mouse[SDL_BUTTON_MIDDLE] == 1 && stateEvent == EVENT_FOR_EDITOR)
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
        if(event.mouse[SDL_BUTTON_WHEELDOWN] == 1 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            zoomModel += 0.05;
            if(zoomModel > 10)
                zoomModel = 10;
        }
        if(event.mouse[SDL_BUTTON_WHEELUP] == 1 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            zoomModel -= 0.05;
            if(zoomModel < 1)
                zoomModel = 1;
        }
        if(oldDimensionResized != dimensionResized && event.mouse[SDL_BUTTON_RIGHT] == 1)
        {
            modeSculpting = CLIC_AND_SLIDE;
        }
        if(event.mouse[SDL_BUTTON_RIGHT] == 0 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            if(selected == 1 && modeSculpting == CLIC_AND_SLIDE)
            {
                if(oldDimensionResized != dimensionResized)
                {
                    selected = 0;
                    oldDimensionResized = dimensionResized;
                    modeSculpting = CLIC_SLIDE_CLIC;
                }
                else
                {
                    modeSculpting = CLIC_SLIDE_CLIC;
                }
            }
        }
        if(event.mouse[SDL_BUTTON_RIGHT] == 1 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            if(selected != 1 && indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6)
            {
                modeSculpting = CLIC_AND_SLIDE;
                oldDimensionResized = dimensionResized;
                selected = 1;
            }

            if(selected == 1 && modeSculpting == CLIC_SLIDE_CLIC)
            {
                selected = 0;
                oldDimensionResized = dimensionResized;
                event.mouse[SDL_BUTTON_RIGHT] = 0;
            }
        }
        if(event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            if(areaTexSelected[0].x >= 0 && selected == 0)
            {
                if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION && areaTexSelected[0].x != -1)
                {
                    putTextureOnModel(&model, indexMemberAffected, indexFaceAffected, areaTexSelected);
                }
            }

            if(buttonSelected == 0)
            {
                selected = 0;
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
        {
            dimensionResized = editCube(&model, modeModelisation, indexMemberAffected, indexFaceAffected, selected);
            model.saved = 0;
        }

        if(mainButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(buttonsToRender != FILE_BUTTONS)
                buttonsToRender = FILE_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(mainButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(buttonsToRender != EDITION_BUTTONS)
                buttonsToRender = EDITION_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(mainButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(buttonsToRender != TOOL_BUTTONS)
                buttonsToRender = TOOL_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(mainButton[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(buttonsToRender != TEXTURE_BUTTONS)
                buttonsToRender = TEXTURE_BUTTONS;
            else
                buttonsToRender = MAIN_BUTTONS;

            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(buttonsToRender == FILE_BUTTONS)
        {
            if(fileButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(openBrowser(buff.string, MODELS))
                {
                    createModel(mainPath, buff.string, &model);
                    sprintf(pathModel, buff.string);
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(fileButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(saveModel(pathModel, &model))
                {
                    model.saved = 1;
                    addStringToText(&fileButton[1].text, "Saved");
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(fileButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(openBrowser(buff.string, MODELS))
                {
                    sprintf(pathModel, buff.string);
                    if(saveModel(buff.string, &model))
                    {
                        model.saved = 1;
                        addStringToText(&fileButton[1].text, "Saved");
                    }
                }

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(fileButton[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(editAnimations(&model) == 0)
                {
                    leave = 1;
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        if(buttonsToRender == EDITION_BUTTONS || ((event.keydown[SDLK_LCTRL] == 1 || event.keydown[SDLK_RCTRL] == 1) && (event.keydown[SDLK_c] == 1 || event.keydown[SDLK_v] == 1)))
        {
            if((editionButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR) || ((event.keydown[SDLK_LCTRL] == 1 || event.keydown[SDLK_RCTRL] == 1) && event.keydown[SDLK_c] == 1))
            {
                if(selected == 1 && selection == CUBE_SELECTION)
                {
                    copyCube(&model, indexMemberAffected, &cubeCopied, &translationCopied, &rotationCopied);
                    copied = 1;
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
                event.keydown[SDLK_c] = 0;
            }

            if((editionButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR) || ((event.keydown[SDLK_LCTRL] == 1 || event.keydown[SDLK_RCTRL] == 1) && event.keydown[SDLK_v] == 1))
            {
                if(copied == 1)
                {
                    pasteCube(&model, &cubeCopied, &translationCopied, &rotationCopied);
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
                event.keydown[SDLK_v] = 0;
            }
            if(editionButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(modeModelisation == SCULPT_MODE)
                {
                    modeModelisation = TRANSLATION_MODE;
                    selection = FACE_SELECTION;
                    addStringToText(&editionButton[2].text, "Translation Mode");
                    addStringToText(&toolButton[2].text, "Selection : Face");
                    selected = 0;
                }
                else
                {
                    modeModelisation = SCULPT_MODE;
                    addStringToText(&editionButton[2].text, "Sculpt Mode");
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        if(buttonsToRender == TOOL_BUTTONS)
        {
            if(toolButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(model.nbMembers < MEMBERS_MAX)
                {
                    addCube(&model);
                    model.saved = 0;
                    event.mouse[SDL_BUTTON_LEFT] = 0;
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            if(toolButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(selected == 1 && selection == CUBE_SELECTION)
                {
                    removeCube(&model, &indexMemberAffected);
                    model.saved = 0;
                    selected = 0;
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(toolButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(selected == 1 && selection == CUBE_SELECTION)
                {
                    removeCube(&model, &indexMemberAffected);
                    model.saved = 0;
                    selected = 0;
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if(toolButton[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(selection == FACE_SELECTION)
                {
                    addStringToText(&toolButton[2].text, "Selection : Cube");
                    selection = CUBE_SELECTION;
                    selected = 0;
                }
                else if(selection == CUBE_SELECTION)
                {
                    addStringToText(&toolButton[2].text, "Selection : Face");
                    selection = FACE_SELECTION;
                    selected = 0;
                }
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        if(buttonsToRender == TEXTURE_BUTTONS || event.keydown[SDLK_RSHIFT] == 1)
        {
            if(textureButton[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(openBrowser(buff.string, TEXTURES))
                {
                    getNameFileFromPath(mainPath, buff.string, model.tex.path);
                    openTexture(mainPath, &model);
                }

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }

            if(textureButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
            {
                if(axisReversing == X_AXIS)
                {
                    addStringToText(&textureButton[1].text, "Invert Texture : Y");
                    axisReversing = Y_AXIS;
                }
                else
                {
                    addStringToText(&textureButton[1].text, "Invert Texture : X");
                    axisReversing = X_AXIS;
                }

                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
            if((textureButton[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR) || event.keydown[SDLK_RSHIFT] == 1)
            {
                if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
                {
                    reverseTexture(&model, axisReversing, indexMemberAffected, indexFaceAffected);
                }
                event.keydown[SDLK_RSHIFT] = 0;
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }

        if(event.keydown[SDLK_ESCAPE] == 1 || event.mouse[SDL_BUTTON_LEFT] == 1)
        {
            buttonsToRender = MAIN_BUTTONS;
        }

        if(event.keydown[SDLK_DELETE] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(selected == 1 && selection == CUBE_SELECTION)
            {
                removeCube(&model, &indexMemberAffected);
                model.saved = 0;
                selected = 0;
            }
            event.keydown[SDLK_DELETE] = 0;
        }

        if(stateEvent == EVENT_FOR_STRING && event.keydown[SDLK_ESCAPE] == 1)
        {
            stateEvent = EVENT_FOR_EDITOR;
            SDL_EnableUNICODE(0);
            event.keydown[SDLK_ESCAPE] = 0;

            for(i = 0; i < NUMBER_MAIN_BUTTONS; i++)
            {
                mainButton[i].textInput = 0;
            }
        }

        if(model.tex.IDtex != 0 && event.posX >= textureEditor.pos.x && stateEvent == EVENT_FOR_EDITOR)
        {
            moveAndResizeTexture(&model.tex, &zoomTexEditor, textureEditor.pos.x, &differenceEventX, &differenceEventY);
            selectAreaTex(&model.tex, areaTexSelected, &selectioningTex);
            areaTexSelected[0].x = (int)areaTexSelected[0].x;
            areaTexSelected[0].y = (int)areaTexSelected[0].y;
            areaTexSelected[1].x = (int)areaTexSelected[1].x;
            areaTexSelected[1].y = (int)areaTexSelected[1].y;
            selectionTexture.point[0].x = areaTexSelected[0].x / model.tex.wMax * model.tex.weight + model.tex.pos.x;
            selectionTexture.point[0].y = windowHeight - (areaTexSelected[0].y / model.tex.hMax * model.tex.height + model.tex.pos.y);
            selectionTexture.point[1].x = areaTexSelected[1].x / model.tex.wMax * model.tex.weight + model.tex.pos.x;
            selectionTexture.point[1].y = windowHeight - (areaTexSelected[1].y / model.tex.hMax * model.tex.height + model.tex.pos.y);
        }

        actualTicks = SDL_GetTicks();

        if(actualTicks - previousTicks > 15)
        {
            moveCamera(&pos, angleX, angleY, &zoomModel);

            if(!selected && event.posX < textureEditor.pos.x)
            {
                clearScene();
                modeRender(RENDER_3D, &pos, &target, FOV);
                renderModel(&model, COLLISION_MODE);

                collisionCursorModel(&model, &indexMemberAffected, &indexFaceAffected);
            }
            if(!selected && event.posX >= textureEditor.pos.x)
            {
                    indexMemberAffected = -1;
                    indexFaceAffected = -1;
            }
            if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
            {
                model.member[indexMemberAffected]->face[indexFaceAffected].color.r = 241;
                model.member[indexMemberAffected]->face[indexFaceAffected].color.v = 248;
                model.member[indexMemberAffected]->face[indexFaceAffected].color.b = 12;
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == CUBE_SELECTION)
            {
                for(i = 0; i < 6; i++)
                {
                    model.member[indexMemberAffected]->face[i].color.r = 100 + i * 20;
                    model.member[indexMemberAffected]->face[i].color.v = 100 + i * 20;
                    model.member[indexMemberAffected]->face[i].color.b = 100 + i * 20;
                }
            }

            clearScene();

            modeRender(RENDER_3D, &pos, &target, FOV);
            renderModel(&model, RENDER_MODE);

            sprintf(textDimensionResized.string, "%lf", dimensionResized);
            textDimensionResized.nbChar = 8;

            modeRender(RENDER_2D, &pos, &target, FOV);
            renderMenuEditor(&model, mainButton, editionButton, fileButton, toolButton, textureButton, buttonsToRender, &texText, weightLetter, &buff, &textureEditor, &selectionTexture, &textDimensionResized);

            refreshScene();
            if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
            {
                if(model.member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture.x == -1)
                {
                    model.member[indexMemberAffected]->face[indexFaceAffected].color.r = indexFaceAffected * 20 + 50;
                    model.member[indexMemberAffected]->face[indexFaceAffected].color.v = indexFaceAffected * 20 + 50;
                    model.member[indexMemberAffected]->face[indexFaceAffected].color.b = indexFaceAffected * 20 + 50;
                }
                else
                {
                    model.member[indexMemberAffected]->face[indexFaceAffected].color.r = 255;
                    model.member[indexMemberAffected]->face[indexFaceAffected].color.v = 255;
                    model.member[indexMemberAffected]->face[indexFaceAffected].color.b = 255;
                }
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == CUBE_SELECTION)
            {
                if(model.member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture.x == -1)
                {
                    for(i = 0; i < 6; i++)
                    {
                        model.member[indexMemberAffected]->face[i].color.r = i * 20 + 50;
                        model.member[indexMemberAffected]->face[i].color.v = i * 20 + 50;
                        model.member[indexMemberAffected]->face[i].color.b = i * 20 + 50;
                    }
                }
                else
                {
                    for(i = 0; i < 6; i++)
                    {
                        model.member[indexMemberAffected]->face[i].color.r = 255;
                        model.member[indexMemberAffected]->face[i].color.v = 255;
                        model.member[indexMemberAffected]->face[i].color.b = 255;
                    }
                }
            }

            if(stateEvent == EVENT_FOR_STRING)
            {
                addCharToString(&buff, getCharFromKeyboard());
            }

            if(model.saved == 0 && strstr(fileButton[1].text.string, "Save Model") == NULL)
            {
                addStringToText(&fileButton[1].text, "Save Model");
            }

            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }
    }

    if(model.nbMembers != 0)
    {
        freeModel(&model);
    }

    free(mainButton);
    free(editionButton);
    free(fileButton);
    free(toolButton);
    free(textureButton);

    return 1;
}

int addCube(Model *model)
{
    if(model->nbMembers < MEMBERS_MAX)
    {
        model->member[model->nbMembers] = malloc(sizeof(Cube));
        model->translation[model->nbMembers] = malloc(sizeof(Point3D));
        model->rotation[model->nbMembers] = malloc(sizeof(Point3D));
        if(model->member[model->nbMembers] == NULL || model->translation[model->nbMembers] == NULL || model->rotation[model->nbMembers] == NULL)
        {
            printf("Error allocating memory\n");
        }
        model->nbMembers++;
        initCube(model->member[model->nbMembers - 1]);
        model->translation[model->nbMembers - 1]->x = 0;
        model->translation[model->nbMembers - 1]->y = 0;
        model->translation[model->nbMembers - 1]->z = 0;

        model->rotation[model->nbMembers - 1]->x = 0;
        model->rotation[model->nbMembers - 1]->y = 0;
        model->rotation[model->nbMembers - 1]->z = 0;
    }
    return 1;
}

int removeCube(Model *model, int *indexMemberAffected)
{
    int i;

    free(model->member[(*indexMemberAffected)]);
    free(model->translation[(*indexMemberAffected)]);
    free(model->rotation[(*indexMemberAffected)]);

    for(i = (*indexMemberAffected); i < model->nbMembers - 1; i++)
    {
        model->member[i] = model->member[i + 1];
        model->translation[i] = model->translation[i + 1];
        model->rotation[i] = model->rotation[i + 1];
    }

    model->nbMembers--;

    return 1;
}

int initCube(Cube *cube)
{
    int i, j;

    for(i = 0; i < 6; i++)
    {
        cube->face[i].color.r = i * 20 + 50;
        cube->face[i].color.v = i * 20 + 50;
        cube->face[i].color.b = i * 20 + 50;

        for(j = 0; j < 4; j++)
        {
            cube->face[i].point[j].x = 0;
            cube->face[i].point[j].y = 0;
            cube->face[i].point[j].z = 0;

            cube->face[i].point[j].coordFileTexture.x = -1;
            cube->face[i].point[j].coordFileTexture.y = -1;
        }
    }

    cube->face[0].point[1].y = 1;
    cube->face[0].point[2].x = 1;
    cube->face[0].point[2].y = 1;
    cube->face[0].point[3].x = 1;

    cube->face[1].point[0].z = 1;
    cube->face[1].point[1].y = 1;
    cube->face[1].point[1].z = 1;
    cube->face[1].point[2].y = 1;

    cube->face[2].point[0].x = 1;
    cube->face[2].point[0].z = 1;
    cube->face[2].point[1].x = 1;
    cube->face[2].point[1].y = 1;
    cube->face[2].point[1].z = 1;
    cube->face[2].point[2].z = 1;
    cube->face[2].point[2].y = 1;
    cube->face[2].point[3].z = 1;

    cube->face[3].point[0].x = 1;
    cube->face[3].point[1].x = 1;
    cube->face[3].point[1].y = 1;
    cube->face[3].point[2].x = 1;
    cube->face[3].point[2].y = 1;
    cube->face[3].point[2].z = 1;
    cube->face[3].point[3].x = 1;
    cube->face[3].point[3].z = 1;

    cube->face[4].point[0].z = 1;
    cube->face[4].point[2].x = 1;
    cube->face[4].point[3].x = 1;
    cube->face[4].point[3].z = 1;

    cube->face[5].point[0].y = 1;
    cube->face[5].point[1].y = 1;
    cube->face[5].point[1].x = 1;
    cube->face[5].point[2].x = 1;
    cube->face[5].point[2].y = 1;
    cube->face[5].point[2].z = 1;
    cube->face[5].point[3].y = 1;
    cube->face[5].point[3].z = 1;

    return 1;
}

void moveCamera(Point3D *pos, double angleX, double angleY, double *zoom)
{
    pos->x = sin(M_PI / 180 * angleY) * sin(M_PI / 180 * angleX) * (*zoom);
    pos->z = cos(M_PI / 180 * angleY) * sin(M_PI / 180 * angleX) * (*zoom);
    pos->y = cos(M_PI / 180 * angleX) * (*zoom);
}

void collisionCursorModel(Model *model, int *indexMemberAffected, int *indexFaceAffected)
{
    GLubyte pixel[3];

    glReadPixels(event.posX, windowHeight - event.posY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    if(pixel[0] != 0)
    {
        (*indexFaceAffected) = pixel[1] - 10;
        (*indexMemberAffected) = pixel[0] - 10;
    }
    else
    {
        (*indexFaceAffected) = -1;
        (*indexMemberAffected) = -1;
    }
}

double editCube(Model *model, int modeModelisation, int indexMemberAffected, int indexFaceAffected, int selected)
{
    int coordEdited = -1;
    int i, j, k;
    double dimensionEdited = 0;
    Point3D tmp;

    if(model->member[indexMemberAffected]->face[indexFaceAffected].point[0].x == model->member[indexMemberAffected]->face[indexFaceAffected].point[2].x)
        coordEdited = X;
    else if(model->member[indexMemberAffected]->face[indexFaceAffected].point[0].y == model->member[indexMemberAffected]->face[indexFaceAffected].point[2].y)
        coordEdited = Y;
    else if(model->member[indexMemberAffected]->face[indexFaceAffected].point[0].z == model->member[indexMemberAffected]->face[indexFaceAffected].point[2].z)
        coordEdited = Z;

    switch(coordEdited)
    {
        case X:
            for(i = 0; i < 4; i++)
            {
                if(modeModelisation == SCULPT_MODE)
                {
                    dimensionEdited = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].x - model->member[indexMemberAffected]->face[(indexFaceAffected + 2) % 4].point[i].x;

                    if(dimensionEdited < 0)
                        dimensionEdited *= -1;
                }
                else if(modeModelisation == TRANSLATION_MODE)
                {
                    dimensionEdited = model->translation[indexMemberAffected]->x;
                }

                if(selected)
                {
                    if(modeModelisation == SCULPT_MODE)
                    {
                        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[i];
                        model->member[indexMemberAffected]->face[indexFaceAffected].point[i].x += event.xrel * 0.005;

                        for(j = 0; j < 6; j++)
                        {
                            for(k = 0; k < 4; k++)
                            {
                                if(model->member[indexMemberAffected]->face[j].point[k].x == tmp.x && model->member[indexMemberAffected]->face[j].point[k].y == tmp.y && model->member[indexMemberAffected]->face[j].point[k].z == tmp.z)
                                {
                                   model->member[indexMemberAffected]->face[j].point[k].x = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].x;
                                   model->member[indexMemberAffected]->face[j].point[k].y = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].y;
                                   model->member[indexMemberAffected]->face[j].point[k].z = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].z;
                                }
                            }
                        }
                    }
                    else if(modeModelisation == TRANSLATION_MODE)
                    {
                        model->translation[indexMemberAffected]->x += event.xrel * 0.00125;
                    }
                }
            }
            break;
        case Y:
            for(i = 0; i < 4; i++)
            {
                if(modeModelisation == SCULPT_MODE)
                {
                    dimensionEdited = model->member[indexMemberAffected]->face[4].point[i].y - model->member[indexMemberAffected]->face[5].point[i].y;

                    if(dimensionEdited < 0)
                        dimensionEdited *= -1;
                }
                else if(modeModelisation == TRANSLATION_MODE)
                {
                    dimensionEdited = model->translation[indexMemberAffected]->y;
                }

                if(selected)
                {
                    if(modeModelisation == SCULPT_MODE)
                    {
                        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[i];
                        model->member[indexMemberAffected]->face[indexFaceAffected].point[i].y -= event.yrel * 0.005;

                        for(j = 0; j < 6; j++)
                        {
                            for(k = 0; k < 4; k++)
                            {
                                if(model->member[indexMemberAffected]->face[j].point[k].x == tmp.x && model->member[indexMemberAffected]->face[j].point[k].y == tmp.y && model->member[indexMemberAffected]->face[j].point[k].z == tmp.z)
                                {
                                   model->member[indexMemberAffected]->face[j].point[k].x = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].x;
                                   model->member[indexMemberAffected]->face[j].point[k].y = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].y;
                                   model->member[indexMemberAffected]->face[j].point[k].z = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].z;
                                }
                            }
                        }
                    }
                    else if(modeModelisation == TRANSLATION_MODE)
                    {
                        model->translation[indexMemberAffected]->y -= event.yrel * 0.00125;
                    }
                }
            }
            break;
        case Z:
            for(i = 0; i < 4; i++)
            {
                if(modeModelisation == SCULPT_MODE)
                {
                    dimensionEdited = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].z - model->member[indexMemberAffected]->face[(indexFaceAffected + 2) % 4].point[i].z;

                    if(dimensionEdited < 0)
                        dimensionEdited *= -1;
                }
                else if(modeModelisation == TRANSLATION_MODE)
                {
                    dimensionEdited = model->translation[indexMemberAffected]->z;
                }

                if(selected)
                {
                    if(modeModelisation == SCULPT_MODE)
                    {
                        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[i];
                        model->member[indexMemberAffected]->face[indexFaceAffected].point[i].z += event.xrel * 0.005;

                        for(j = 0; j < 6; j++)
                        {
                            for(k = 0; k < 4; k++)
                            {
                                if(model->member[indexMemberAffected]->face[j].point[k].x == tmp.x && model->member[indexMemberAffected]->face[j].point[k].y == tmp.y && model->member[indexMemberAffected]->face[j].point[k].z == tmp.z)
                                {
                                   model->member[indexMemberAffected]->face[j].point[k].x = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].x;
                                   model->member[indexMemberAffected]->face[j].point[k].y = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].y;
                                   model->member[indexMemberAffected]->face[j].point[k].z = model->member[indexMemberAffected]->face[indexFaceAffected].point[i].z;
                                }
                            }
                        }
                    }
                    else if(modeModelisation == TRANSLATION_MODE)
                    {
                        model->translation[indexMemberAffected]->z += event.xrel * 0.00125;
                    }
                }
            }
            break;
    }

    return dimensionEdited;
}

void renderMenuEditor(Model *model, Button *mainButton, Button *editionButton, Button *fileButton, Button *toolButton, Button *textureButton, int buttonsToRender, Texture *textureText, int *weightLetter, Text *buff, Texture *textureEditor, Face *selectionTexture, Text *textDimensionResized)
{
    int i;

    glPushMatrix();
    glTranslated(0, 0, -100);
    drawTexture(textureEditor);

    for(i = 0; i < NUMBER_MAIN_BUTTONS; i++)
    {
        renderButton(&mainButton[i], textureText, weightLetter, buff);
    }

    if(buttonsToRender == EDITION_BUTTONS)
    {
        for(i = 0; i < NUMBER_EDITION_BUTTONS; i++)
        {
            renderButton(&editionButton[i], textureText, weightLetter, buff);
        }
    }

    if(buttonsToRender == FILE_BUTTONS)
    {
        for(i = 0; i < NUMBER_FILE_BUTTONS; i++)
        {
            renderButton(&fileButton[i], textureText, weightLetter, buff);
        }
    }

    if(buttonsToRender == TOOL_BUTTONS)
    {
        for(i = 0; i < NUMBER_TOOL_BUTTONS; i++)
        {
            renderButton(&toolButton[i], textureText, weightLetter, buff);
        }
    }

    if(buttonsToRender == TEXTURE_BUTTONS)
    {
        for(i = 0; i < NUMBER_TEXTURE_BUTTONS; i++)
        {
            renderButton(&textureButton[i], textureText, weightLetter, buff);
        }
    }

    glTranslated(0, 0, -1);
    drawTexture(&model->tex);
    glTranslated(0, 0, 1);

    glBegin(GL_QUADS);
    glVertex2d(selectionTexture->point[0].x, selectionTexture->point[0].y);
    glVertex2d(selectionTexture->point[1].x, selectionTexture->point[0].y);
    glVertex2d(selectionTexture->point[1].x, selectionTexture->point[1].y);
    glVertex2d(selectionTexture->point[0].x, selectionTexture->point[1].y);
    glEnd();

    writeText(textureText, *textDimensionResized, weightLetter, textureEditor->pos.x - 100, windowHeight - 20);

    glPopMatrix();
}

int moveAndResizeTexture(Texture *tex, double *zoom, int xMin, int *differenceEventX, int *differenceEventY)
{
    int tmp;

    if(collisionCursorTexture(tex))
    {
        if(event.mouse[SDL_BUTTON_WHEELDOWN] == 1)
        {
            if((*zoom) > 0.5)
                (*zoom) -= 0.5;

            tex->weight = (*zoom) * (double)tex->wMax;
            tex->height = (*zoom) * (double)tex->hMax;
            event.mouse[SDL_BUTTON_WHEELDOWN] = 0;
        }
        if(event.mouse[SDL_BUTTON_WHEELUP] == 1)
        {
            (*zoom) += 0.5;
            tex->weight = (*zoom) * (double)tex->wMax;
            tex->height = (*zoom) * (double)tex->hMax;
            event.mouse[SDL_BUTTON_WHEELDOWN] = 0;
        }
        if(event.mouse[SDL_BUTTON_RIGHT] == 1)
        {
            if((*differenceEventX) == -1)
            {
                (*differenceEventX) = event.posX - tex->pos.x;
                (*differenceEventY) = event.posY - tex->pos.y;
            }

            tmp = tex->weight;
            tex->weight = (*zoom) * (double)tex->wMax;
            tex->posTex[0].x = 0;

            tex->pos.x = event.posX - (*differenceEventX) - tex->weight + tmp;
            tex->pos.y = event.posY - (*differenceEventY);

            if(tex->pos.x < xMin || tex->weight < 16)
            {
                tex->weight = tex->weight - (xMin - tex->pos.x);
                tex->posTex[0].x = ((*zoom) * (double)tex->wMax - tex->weight) / ((*zoom) * (double)tex->wMax);
                tex->pos.x = xMin;
                (*differenceEventX) = event.posX - tex->pos.x;
            }
        }
        else
        {
            (*differenceEventX) = -1;
            (*differenceEventY) = -1;
        }
    }
    return 1;
}

int selectAreaTex(Texture *tex, Point2D coordsArea[2], int *selectioning)
{
    if(tex->path[0] != 0)
    {
        if(!collisionCursorTexture(tex) && event.mouse[SDL_BUTTON_LEFT] == 1 && !(*selectioning))
        {
            coordsArea[0].x = -1;
            coordsArea[0].y = -1;
            coordsArea[1].x = -1;
            coordsArea[1].y = -1;
        }

        if(collisionCursorTexture(tex))
        {
            if(event.mouse[SDL_BUTTON_LEFT] == 1)
            {
                if((*selectioning == 0))
                {
                    (*selectioning) = 1;
                    coordsArea[0].x = ((event.posX - tex->pos.x) * tex->wMax) / tex->weight;
                    coordsArea[0].y = ((event.posY - tex->pos.y) * tex->hMax) / tex->height;
                }
            }
            if((*selectioning == 1 && event.mouse[SDL_BUTTON_LEFT] == 1))
            {
                coordsArea[1].x = ((event.posX - tex->pos.x) * tex->wMax) / tex->weight + 1;
                coordsArea[1].y = ((event.posY - tex->pos.y) * tex->hMax) / tex->height + 1;
            }
        }
        if(event.mouse[SDL_BUTTON_LEFT] == 0)
        {
            (*selectioning) = 0;
        }
    }

    return 1;
}

int collisionCursorTexture(Texture *tex)
{
    if(event.posX >= tex->pos.x && event.posX <= tex->pos.x + tex->weight)
    {
        if(event.posY >= tex->pos.y && event.posY <= tex->pos.y + tex->height)
        {
            return 1;
        }
    }

    return 0;
}

int putTextureOnModel(Model *model, int indexMemberAffected, int indexFaceAffected, Point2D areaTexSelected[2])
{
    model->member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture.x = areaTexSelected[1].x / model->tex.wMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture.y = (model->tex.hMax - areaTexSelected[1].y) / model->tex.hMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[1].coordFileTexture.x = areaTexSelected[1].x / model->tex.wMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[1].coordFileTexture.y = (model->tex.hMax - areaTexSelected[0].y) / model->tex.hMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[2].coordFileTexture.x = areaTexSelected[0].x / model->tex.wMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[2].coordFileTexture.y = (model->tex.hMax - areaTexSelected[0].y) / model->tex.hMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[3].coordFileTexture.x = areaTexSelected[0].x / model->tex.wMax;
    model->member[indexMemberAffected]->face[indexFaceAffected].point[3].coordFileTexture.y = (model->tex.hMax - areaTexSelected[1].y) / model->tex.hMax;

    model->member[indexMemberAffected]->face[indexFaceAffected].color.r = 255;
    model->member[indexMemberAffected]->face[indexFaceAffected].color.v = 255;
    model->member[indexMemberAffected]->face[indexFaceAffected].color.b = 255;

    return 1;
}

void reverseTexture(Model *model, int axisReversing, int indexMemberAffected, int indexFaceAffected)
{
    Point2D tmp;

    if(axisReversing == X_AXIS)
    {
        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture = model->member[indexMemberAffected]->face[indexFaceAffected].point[3].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[3].coordFileTexture = tmp;

        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[1].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[1].coordFileTexture = model->member[indexMemberAffected]->face[indexFaceAffected].point[2].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[2].coordFileTexture = tmp;
    }
    else
    {
        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[0].coordFileTexture = model->member[indexMemberAffected]->face[indexFaceAffected].point[1].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[1].coordFileTexture = tmp;

        tmp = model->member[indexMemberAffected]->face[indexFaceAffected].point[2].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[2].coordFileTexture = model->member[indexMemberAffected]->face[indexFaceAffected].point[3].coordFileTexture;
        model->member[indexMemberAffected]->face[indexFaceAffected].point[3].coordFileTexture = tmp;
    }
}

int copyCube(Model *model, int indexMemberSelected, Cube *cubeCopied, Point3D *translationCopied, Point3D *rotationCopied)
{
    (*cubeCopied) = (*model->member[indexMemberSelected]);
    (*translationCopied) = (*model->translation[indexMemberSelected]);
    (*rotationCopied) = (*model->rotation[indexMemberSelected]);

    return 1;
}

int pasteCube(Model *model, Cube *cubeCopied, Point3D *translationCopied, Point3D *rotationCopied)
{
    if(model->nbMembers < MEMBERS_MAX)
    {
        model->member[model->nbMembers] = malloc(sizeof(Cube));
        model->translation[model->nbMembers] = malloc(sizeof(Point3D));
        model->rotation[model->nbMembers] = malloc(sizeof(Point3D));

        if(model->member[model->nbMembers] != NULL && model->translation[model->nbMembers] != NULL && model->rotation[model->nbMembers] != NULL)
        {
            (*model->member[model->nbMembers]) = (*cubeCopied);
            (*model->translation[model->nbMembers]) = (*translationCopied);
            (*model->rotation[model->nbMembers]) = (*rotationCopied);
        }
        model->nbMembers++;

        return 1;
    }

    return 0;
}
