#include "constants.h"
#include "editor.h"
#include "model.h"
#include "render.h"
#include "input.h"
#include "sdlglutils.h"
#include "button.h"
#include "text.h"

extern Input event;

int editor()
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
    int weightLetter[256];
    int selection = FACE_SELECTION;
    int stateEvent = EVENT_FOR_EDITOR;
    int textInput = 0;
    int axisReversing = X_AXIS;

    double dimensionResized = -1;
    Text textDimensionResized;

    Text path;

    Point3D pos, target;
    Model model;

    Button *button;
    Texture texButton;
    Texture texText;
    Texture textureEditor;

    buff.shadow = 1;

    button = malloc(NUMBER_BUTTONS_EDITOR * sizeof(Button));
    if(button == NULL)
    {
        printf("Error allocating button's memory\n");
    }

    loadWeightLetters("textures/font/default.properties", &weightLetter[0]);

    sprintf(texText.path, "textures/font/default.png");
    loadTexture(&texText);

    sprintf(texButton.path, "textures/gui/gui.png");
    loadTexture(&texButton);

    sprintf(textureEditor.path, "textures/texEditor.png");
    loadTexture(&textureEditor);

    textureEditor.color.r = 255;
    textureEditor.color.v = 255;
    textureEditor.color.b = 255;
    textureEditor.height = WINDOW_HEIGHT;
    textureEditor.weight = textureEditor.wMax;
    textureEditor.pos.x = WINDOW_WEIGHT - textureEditor.weight - 1;
    textureEditor.pos.y = 0;

    selectionTexture.point[0].z = 0;
    selectionTexture.point[1].z = 0;

    areaTexSelected[0].x = -1;
    areaTexSelected[0].y = -1;
    areaTexSelected[1].x = -1;
    areaTexSelected[1].y = -1;

    attribButtons(button, &texButton);

    initModel(&model);

    target.x = 0;
    target.y = 0;
    target.z = 0;

    pos.x = 2;
    pos.y = 2;
    pos.z = 2;

    while(!leave)
    {
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

        for(i = 0; i < NUMBER_BUTTONS_EDITOR; i++)
        {
            if(selected == 0)
            {
                buttonCollision(&button[i]);
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
        if(event.mouse[SDL_BUTTON_RIGHT] == 1 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            if(selected == 1)
            {
                selected = 0;
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6)
            {
                selected = 1;
            }
            event.mouse[SDL_BUTTON_RIGHT] = 0;
        }
        if(event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR && event.posX < textureEditor.pos.x)
        {
            selected = 0;

            if(areaTexSelected[0].x >= 0)
            {
                if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION && areaTexSelected[0].x != -1)
                {
                    putTextureOnModel(&model, indexMemberAffected, indexFaceAffected, areaTexSelected);
                }
            }
        }
        if((event.keydown[SDLK_LCTRL] == 1 || event.keydown[SDLK_RCTRL] == 1) && stateEvent == EVENT_FOR_EDITOR)
        {
            if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
            {
                reverseTexture(&model, axisReversing, indexMemberAffected, indexFaceAffected);
            }
            event.keydown[SDLK_RCTRL] = 0;
            event.keydown[SDLK_LCTRL] = 0;
        }
        if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
        {
            dimensionResized = resizeCube(&model, indexMemberAffected, indexFaceAffected, selected);
            model.saved = 0;
        }

        if(button[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(model.nbMembers < MEMBERS_MAX)
            {
                addCube(&model);
                model.saved = 0;
                event.mouse[SDL_BUTTON_LEFT] = 0;
            }
        }
        if(button[1].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(selection == FACE_SELECTION)
            {
                addStringToText(&button[1].text, "Selection : Cube");
                selection = CUBE_SELECTION;
            }
            else if(selection == CUBE_SELECTION)
            {
                addStringToText(&button[1].text, "Selection : Face");
                selection = FACE_SELECTION;
            }
            event.mouse[SDL_BUTTON_LEFT] = 0;
        }

        if(button[2].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            saveModel(path.string, &model);
            model.saved = 1;
            addStringToText(&button[2].text, "Saved");
            event.mouse[SDL_BUTTON_LEFT] = 0;
        }
        if(button[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            textInput = GET_MODEL_PATH;
            event.mouse[SDL_BUTTON_LEFT] = 0;
            stateEvent = EVENT_FOR_STRING;
            addStringToText(&buff, "");
            SDL_EnableUNICODE(1);
            button[3].textInput = 1;
        }
        if(button[4].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            textInput = GET_TEXTURE_PATH;
            event.mouse[SDL_BUTTON_LEFT] = 0;
            stateEvent = EVENT_FOR_STRING;
            addStringToText(&buff, "");
            model.saved = 0;
            SDL_EnableUNICODE(1);
            button[4].textInput = 1;
        }
        if(button[5].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(axisReversing == X_AXIS)
            {
                addStringToText(&button[5].text, "Invert Texture : Y");
                axisReversing = Y_AXIS;
            }
            else
            {
                addStringToText(&button[5].text, "Invert Texture : X");
                axisReversing = X_AXIS;
            }

            event.mouse[SDL_BUTTON_LEFT] = 0;
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
        if(stateEvent == EVENT_FOR_STRING && event.keydown[SDLK_RETURN] == 1)
        {
            stateEvent = EVENT_FOR_EDITOR;
            SDL_EnableUNICODE(0);
            event.keydown[SDLK_RETURN] = 0;

            if(textInput == GET_MODEL_PATH)
            {
                freeModel(&model);
                createModel(buff.string, &model);
                addStringToText(&path, buff.string);
                button[3].textInput = 0;
            }
            else if(textInput == GET_TEXTURE_PATH)
            {
                sprintf(model.tex.path, "%s", buff.string);
                openTexture(&model);
                button[4].textInput = 0;
            }
        }

        if(stateEvent == EVENT_FOR_STRING && event.keydown[SDLK_ESCAPE] == 1)
        {
            stateEvent = EVENT_FOR_EDITOR;
            SDL_EnableUNICODE(0);
            event.keydown[SDLK_ESCAPE] = 0;

            for(i = 0; i < NUMBER_BUTTONS_EDITOR; i++)
            {
                button[i].textInput = 0;
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
            selectionTexture.point[0].y = WINDOW_HEIGHT - (areaTexSelected[0].y / model.tex.hMax * model.tex.height + model.tex.pos.y);
            selectionTexture.point[1].x = areaTexSelected[1].x / model.tex.wMax * model.tex.weight + model.tex.pos.x;
            selectionTexture.point[1].y = WINDOW_HEIGHT - (areaTexSelected[1].y / model.tex.hMax * model.tex.height + model.tex.pos.y);
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
                model.member[indexMemberAffected].face[indexFaceAffected].color.r = 241;
                model.member[indexMemberAffected].face[indexFaceAffected].color.v = 248;
                model.member[indexMemberAffected].face[indexFaceAffected].color.b = 12;
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == CUBE_SELECTION)
            {
                for(i = 0; i < 6; i++)
                {
                    model.member[indexMemberAffected].face[i].color.r = 100 + i * 20;
                    model.member[indexMemberAffected].face[i].color.v = 100 + i * 20;
                    model.member[indexMemberAffected].face[i].color.b = 100 + i * 20;
                }
            }

            clearScene();

            modeRender(RENDER_3D, &pos, &target, FOV);
            renderModel(&model, RENDER_MODE);

            sprintf(textDimensionResized.string, "%lf", dimensionResized);
            textDimensionResized.nbChar = 8;

            modeRender(RENDER_2D, &pos, &target, FOV);
            renderMenuEditor(&model, button, &texText, weightLetter, &buff, &textureEditor, &selectionTexture, &textDimensionResized);

            refreshScene();
            if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
            {
                if(model.member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture.x == -1)
                {
                    model.member[indexMemberAffected].face[indexFaceAffected].color.r = indexFaceAffected * 20 + 50;
                    model.member[indexMemberAffected].face[indexFaceAffected].color.v = indexFaceAffected * 20 + 50;
                    model.member[indexMemberAffected].face[indexFaceAffected].color.b = indexFaceAffected * 20 + 50;
                }
                else
                {
                    model.member[indexMemberAffected].face[indexFaceAffected].color.r = 255;
                    model.member[indexMemberAffected].face[indexFaceAffected].color.v = 255;
                    model.member[indexMemberAffected].face[indexFaceAffected].color.b = 255;
                }
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < model.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == CUBE_SELECTION)
            {
                for(i = 0; i < 6; i++)
                {
                    model.member[indexMemberAffected].face[i].color.r = i * 20 + 50;
                    model.member[indexMemberAffected].face[i].color.v = i * 20 + 50;
                    model.member[indexMemberAffected].face[i].color.b = i * 20 + 50;
                }
            }

            if(stateEvent == EVENT_FOR_STRING)
            {
                addCharToString(&buff, getCharFromKeyboard());
            }

            if(model.saved == 0 && strstr(button[2].text.string, "Save Model") == NULL)
            {
                addStringToText(&button[2].text, "Save Model");
            }

            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }
    }

    if(model.member != NULL)
    {
        freeModel(&model);
    }
    return 1;
}

int addCube(Model *model)
{
    Model tmp;
    int i;

    if(model->nbMembers > 0)
    {
        initModel(&tmp);

        tmp.nbMembers = model->nbMembers;
        tmp.member = malloc(tmp.nbMembers * sizeof(Cube));
        tmp.translation = malloc(tmp.nbMembers * sizeof(Point3D));

        if(tmp.member == NULL || tmp.translation == NULL)
        {
            printf("Error allocating memory 1\n");
            return 0;
        }

        for(i = 0; i < tmp.nbMembers; i++)
        {
            tmp.member[i] = model->member[i];
            tmp.translation[i] = model->translation[i];
        }

        model->nbMembers = tmp.nbMembers + 1;
        model->member = realloc(model->member, model->nbMembers * sizeof(Cube));
        model->translation = realloc(model->translation, model->nbMembers * sizeof(Point3D));

        if(model->member == NULL)
        {
            printf("Error allocating memory 2 (member)\n");
            printf("errno = %d\n", errno);
            return 0;
        }
        if(model->translation == NULL)
        {
            printf("Error allocating memory 2 (translation)\n");
            return 0;
        }

        for(i = 0; i < tmp.nbMembers; i++)
        {
            model->member[i] = tmp.member[i];
            model->translation[i] = tmp.translation[i];
        }

        freeModel(&tmp);
    }

    else
    {
        model->nbMembers++;
        model->member = malloc(model->nbMembers * sizeof(Cube));
        model->translation = malloc(model->nbMembers * sizeof(Point3D));
    }

    model->translation[model->nbMembers - 1].x = 0;
    model->translation[model->nbMembers - 1].y = 0;
    model->translation[model->nbMembers - 1].z = 0;
    initCube(&model->member[model->nbMembers - 1]);

    return 1;
}

int removeCube(Model *model, int *indexMemberAffected)
{
    Model tmp;
    int i;

    initModel(&tmp);

    tmp.nbMembers = model->nbMembers;
    tmp.member = malloc(tmp.nbMembers * sizeof(Cube));
    tmp.translation = malloc(tmp.nbMembers * sizeof(Point3D));

    if(tmp.member == NULL || tmp.translation == NULL)
    {
        printf("Error allocating memory\n");
        return 0;
    }

    tmp = (*model);
    freeModel(model);

    initModel(model);
    model->nbMembers = tmp.nbMembers - 1;
    model->member = malloc(model->nbMembers * sizeof(Cube));
    model->translation = malloc(model->nbMembers * sizeof(Point3D));

    if(model->member == NULL || model->translation == NULL)
    {
        printf("Error allocating memory\n");
        freeModel(model);
        freeModel(&tmp);
        return 0;
    }

    for(i = 0; i < (*indexMemberAffected); i++)
    {
        model->member[i] = tmp.member[i];
        model->translation[i] = tmp.translation[i];
    }
    for(i = (*indexMemberAffected); i < model->nbMembers; i++)
    {
        model->member[i] = tmp.member[i + 1];
        model->translation[i] = tmp.translation[i + 1];
    }

    freeModel(&tmp);

    (*indexMemberAffected) = -1;

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

    glReadPixels(event.posX, WINDOW_HEIGHT - event.posY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

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

double resizeCube(Model *model, int indexMemberAffected, int indexFaceAffected, int selected)
{
    int coordResized = -1;
    int i, j, k;
    double dimensionResized = 0;
    Point3D tmp;

    if(model->member[indexMemberAffected].face[indexFaceAffected].point[0].x == model->member[indexMemberAffected].face[indexFaceAffected].point[2].x)
        coordResized = X;
    else if(model->member[indexMemberAffected].face[indexFaceAffected].point[0].y == model->member[indexMemberAffected].face[indexFaceAffected].point[2].y)
        coordResized = Y;
    else if(model->member[indexMemberAffected].face[indexFaceAffected].point[0].z == model->member[indexMemberAffected].face[indexFaceAffected].point[2].z)
        coordResized = Z;

    switch(coordResized)
    {
        case X:
            for(i = 0; i < 4; i++)
            {
                if(selected)
                {
                    tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                    model->member[indexMemberAffected].face[indexFaceAffected].point[i].x += event.xrel * 0.005;
                }

                dimensionResized = model->member[indexMemberAffected].face[indexFaceAffected].point[i].x - model->member[indexMemberAffected].face[(indexFaceAffected + 2) % 4].point[i].x;

                if(dimensionResized < 0)
                    dimensionResized *= -1;

                if(selected)
                {
                    for(j = 0; j < 6; j++)
                    {
                        for(k = 0; k < 4; k++)
                        {
                            if(model->member[indexMemberAffected].face[j].point[k].x == tmp.x && model->member[indexMemberAffected].face[j].point[k].y == tmp.y && model->member[indexMemberAffected].face[j].point[k].z == tmp.z)
                            {
                               model->member[indexMemberAffected].face[j].point[k].x = model->member[indexMemberAffected].face[indexFaceAffected].point[i].x;
                               model->member[indexMemberAffected].face[j].point[k].y = model->member[indexMemberAffected].face[indexFaceAffected].point[i].y;
                               model->member[indexMemberAffected].face[j].point[k].z = model->member[indexMemberAffected].face[indexFaceAffected].point[i].z;
                            }
                        }
                    }
                }
            }
            break;
        case Y:
            for(i = 0; i < 4; i++)
            {
                if(selected)
                {
                    tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                    model->member[indexMemberAffected].face[indexFaceAffected].point[i].y -= event.yrel * 0.005;
                }
                dimensionResized = model->member[indexMemberAffected].face[4].point[i].y - model->member[indexMemberAffected].face[5].point[i].y;

                if(dimensionResized < 0)
                    dimensionResized *= -1;

                if(selected)
                {
                    for(j = 0; j < 6; j++)
                    {
                        for(k = 0; k < 4; k++)
                        {
                            if(model->member[indexMemberAffected].face[j].point[k].x == tmp.x && model->member[indexMemberAffected].face[j].point[k].y == tmp.y && model->member[indexMemberAffected].face[j].point[k].z == tmp.z)
                            {
                               model->member[indexMemberAffected].face[j].point[k].x = model->member[indexMemberAffected].face[indexFaceAffected].point[i].x;
                               model->member[indexMemberAffected].face[j].point[k].y = model->member[indexMemberAffected].face[indexFaceAffected].point[i].y;
                               model->member[indexMemberAffected].face[j].point[k].z = model->member[indexMemberAffected].face[indexFaceAffected].point[i].z;
                            }
                        }
                    }
                }
            }
            break;
        case Z:
            for(i = 0; i < 4; i++)
            {
                if(selected)
                {
                    tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                    model->member[indexMemberAffected].face[indexFaceAffected].point[i].z += event.xrel * 0.005;
                }
                dimensionResized = model->member[indexMemberAffected].face[indexFaceAffected].point[i].z - model->member[indexMemberAffected].face[(indexFaceAffected + 2) % 4].point[i].z;

                if(dimensionResized < 0)
                    dimensionResized *= -1;

                if(selected)
                {
                    for(j = 0; j < 6; j++)
                    {
                        for(k = 0; k < 4; k++)
                        {
                            if(model->member[indexMemberAffected].face[j].point[k].x == tmp.x && model->member[indexMemberAffected].face[j].point[k].y == tmp.y && model->member[indexMemberAffected].face[j].point[k].z == tmp.z)
                            {
                               model->member[indexMemberAffected].face[j].point[k].x = model->member[indexMemberAffected].face[indexFaceAffected].point[i].x;
                               model->member[indexMemberAffected].face[j].point[k].y = model->member[indexMemberAffected].face[indexFaceAffected].point[i].y;
                               model->member[indexMemberAffected].face[j].point[k].z = model->member[indexMemberAffected].face[indexFaceAffected].point[i].z;
                            }
                        }
                    }
                }
            }
            break;
    }

    return dimensionResized;
}

void renderMenuEditor(Model *model, Button *button, Texture *textureText, int *weightLetter, Text *buff, Texture *textureEditor, Face *selectionTexture, Text *textDimensionResized)
{
    int i;

    glPushMatrix();
    glTranslated(0, 0, -100);
    drawTexture(textureEditor);
    for(i = 0; i < NUMBER_BUTTONS_EDITOR; i++)
    {
        renderButton(&button[i], textureText, weightLetter, buff);
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

    writeText(textureText, *textDimensionResized, weightLetter, textureEditor->pos.x - 100, WINDOW_HEIGHT - 20);

    glPopMatrix();
}

int moveAndResizeTexture(Texture *tex, double *zoom, int xMin, int *differenceEventX, int *differenceEventY)
{
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

            tex->pos.x = event.posX - (*differenceEventX);
            tex->pos.y = event.posY - (*differenceEventY);

            if(tex->pos.x <= xMin)
            {
                tex->pos.x = xMin;
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
                coordsArea[1].x = ((event.posX - tex->pos.x) * tex->wMax) / tex->weight;
                coordsArea[1].y = ((event.posY - tex->pos.y) * tex->hMax) / tex->height;
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
    model->member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture.x = areaTexSelected[0].x / model->tex.wMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture.y = (model->tex.hMax - areaTexSelected[0].y) / model->tex.hMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[1].coordFileTexture.x = areaTexSelected[0].x / model->tex.wMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[1].coordFileTexture.y = (model->tex.hMax - areaTexSelected[1].y) / model->tex.hMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[2].coordFileTexture.x = areaTexSelected[1].x / model->tex.wMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[2].coordFileTexture.y = (model->tex.hMax - areaTexSelected[1].y) / model->tex.hMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[3].coordFileTexture.x = areaTexSelected[1].x / model->tex.wMax;
    model->member[indexMemberAffected].face[indexFaceAffected].point[3].coordFileTexture.y = (model->tex.hMax - areaTexSelected[0].y) / model->tex.hMax;

    model->member[indexMemberAffected].face[indexFaceAffected].color.r = 255;
    model->member[indexMemberAffected].face[indexFaceAffected].color.v = 255;
    model->member[indexMemberAffected].face[indexFaceAffected].color.b = 255;

    return 1;
}

void reverseTexture(Model *model, int axisReversing, int indexMemberAffected, int indexFaceAffected)
{
    Point2D tmp;

    if(axisReversing == X_AXIS)
    {
        tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture = model->member[indexMemberAffected].face[indexFaceAffected].point[3].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[3].coordFileTexture = tmp;

        tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[1].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[1].coordFileTexture = model->member[indexMemberAffected].face[indexFaceAffected].point[2].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[2].coordFileTexture = tmp;
    }
    else
    {
        tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[0].coordFileTexture = model->member[indexMemberAffected].face[indexFaceAffected].point[1].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[1].coordFileTexture = tmp;

        tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[2].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[2].coordFileTexture = model->member[indexMemberAffected].face[indexFaceAffected].point[3].coordFileTexture;
        model->member[indexMemberAffected].face[indexFaceAffected].point[3].coordFileTexture = tmp;
    }
}
