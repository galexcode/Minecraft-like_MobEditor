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
    double angleX = 0.01, angleY = 0, zoom = 2;
    int indexMemberAffected = -1, indexFaceAffected = -1;
    int selected = 0;
    int weightLetter[256];
    int selection = FACE_SELECTION;
    int stateEvent = EVENT_FOR_EDITOR;
    Text currentText;

    Point3D pos, target;
    Model human;

    Button *button;
    Texture texButton;
    Texture texText;

    currentText.string = NULL;
    currentText.nbChar = 0;

    button = malloc(NUMBER_BUTTON_EDITOR * sizeof(Button));
    if(button == NULL)
    {
        printf("Error allocating button's memory\n");
    }

    loadWeightLetters("textures/font/default.properties", &weightLetter[0]);

    sprintf(texText.path, "textures/font/default.png");
    loadTexture(&texText);

    sprintf(texButton.path, "textures/gui/gui.png");
    loadTexture(&texButton);

    attribButtons(button, &texButton);

    initModel(&human);
    loadModel("models/human.mclmdl", &human);

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

        for(i = 0; i < NUMBER_BUTTON_EDITOR; i++)
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
        if(event.mouse[SDL_BUTTON_WHEELDOWN] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            zoom += 0.05;
            if(zoom > 10)
                zoom = 10;
        }
        if(event.mouse[SDL_BUTTON_WHEELUP] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            zoom -= 0.05;
            if(zoom < 1)
                zoom = 1;
        }
        if(event.mouse[SDL_BUTTON_RIGHT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(selected == 1)
            {
                selected = 0;
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < human.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6)
            {
                selected = 1;
            }
            event.mouse[SDL_BUTTON_RIGHT] = 0;
        }
        if(event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            selected = 0;
        }
        if(selected == 1)
        {
            if(indexMemberAffected >= 0 && indexMemberAffected < human.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selected && selection == FACE_SELECTION)
            {
                resizeCube(&human, indexMemberAffected, indexFaceAffected);
            }
        }

        if(button[0].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(human.nbMembers < MEMBERS_MAX)
            {
                addCube(&human);
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
            saveModel("models/human.mclmdl", &human);
            event.mouse[SDL_BUTTON_LEFT] = 0;
        }
        if(button[3].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            stateEvent = EVENT_FOR_STRING;
        }
        if(button[4].selected == 1 && event.mouse[SDL_BUTTON_LEFT] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            stateEvent = EVENT_FOR_STRING;
        }
        if(event.keydown[SDLK_DELETE] == 1 && stateEvent == EVENT_FOR_EDITOR)
        {
            if(selected == 1 && selection == CUBE_SELECTION)
            {
                removeCube(&human, &indexMemberAffected);
                selected = 0;
            }
            event.keydown[SDLK_DELETE] = 0;
        }

        actualTicks = SDL_GetTicks();

        if(actualTicks - previousTicks > 15)
        {
            deplacerCamera(&pos, angleX, angleY, &zoom);

            if(!selected)
            {
                clearScene();
                modeRender(RENDER_3D, &pos, &target, FOV);
                renderModel(&human, COLLISION_MODE);

                collisionCursorModel(&human, &indexMemberAffected, &indexFaceAffected);
            }
            if(indexMemberAffected >= 0 && indexMemberAffected < human.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
            {
                human.member[indexMemberAffected].face[indexFaceAffected].color.r = 255;
                human.member[indexMemberAffected].face[indexFaceAffected].color.v = 255;
                human.member[indexMemberAffected].face[indexFaceAffected].color.b = 255;
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < human.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == CUBE_SELECTION)
            {
                for(i = 0; i < 6; i++)
                {
                    human.member[indexMemberAffected].face[i].color.r = 100 + i * 20;
                    human.member[indexMemberAffected].face[i].color.v = 100 + i * 20;
                    human.member[indexMemberAffected].face[i].color.b = 100 + i * 20;
                }
            }

            clearScene();

            modeRender(RENDER_3D, &pos, &target, FOV);
            renderModel(&human, RENDER_MODE);

            modeRender(RENDER_2D, &pos, &target, FOV);
            renderMenuEditor(button, &texText, weightLetter);

            refreshScene();
            if(indexMemberAffected >= 0 && indexMemberAffected < human.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == FACE_SELECTION)
            {
                human.member[indexMemberAffected].face[indexFaceAffected].color.r = indexFaceAffected * 20 + 50;
                human.member[indexMemberAffected].face[indexFaceAffected].color.v = indexFaceAffected * 20 + 50;
                human.member[indexMemberAffected].face[indexFaceAffected].color.b = indexFaceAffected * 20 + 50;
            }
            else if(indexMemberAffected >= 0 && indexMemberAffected < human.nbMembers && indexFaceAffected >= 0 && indexFaceAffected < 6 && selection == CUBE_SELECTION)
            {
                for(i = 0; i < 6; i++)
                {
                    human.member[indexMemberAffected].face[i].color.r = i * 20 + 50;
                    human.member[indexMemberAffected].face[i].color.v = i * 20 + 50;
                    human.member[indexMemberAffected].face[i].color.b = i * 20 + 50;
                }
            }
            previousTicks = actualTicks;
        }
        else
        {
            SDL_Delay(15 - (actualTicks - previousTicks));
        }
    }

    freeModel(&human);

    return 1;
}

int addCube(Model *model)
{
    Model tmp;
    int i;

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
        attribCube(&model->member[i], &tmp.member[i]);
        model->translation[i] = tmp.translation[i];
    }
    for(i = (*indexMemberAffected); i < model->nbMembers; i++)
    {
        attribCube(&model->member[i], &tmp.member[i + 1]);
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

            cube->face[i].point[j].coordFileTexture.x = 0;
            cube->face[i].point[j].coordFileTexture.y = 0;
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

void deplacerCamera(Point3D *pos, double angleX, double angleY, double *zoom)
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

void resizeCube(Model *model, int indexMemberAffected, int indexFaceAffected)
{
    int coordResized = 0;
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
                tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                model->member[indexMemberAffected].face[indexFaceAffected].point[i].x += event.xrel * 0.005;
                dimensionResized = model->member[indexMemberAffected].face[indexFaceAffected].point[i].x - model->member[indexMemberAffected].face[(indexFaceAffected + 2) % 4].point[i].x;

                if(dimensionResized < 0)
                    dimensionResized *= -1;

                for(j = 0; j < 6; j++)
                {
                    for(k = 0; k < 4; k++)
                    {
                        if(model->member[indexMemberAffected].face[j].point[k].x == tmp.x && model->member[indexMemberAffected].face[j].point[k].y == tmp.y && model->member[indexMemberAffected].face[j].point[k].z == tmp.z)
                        {
                           model->member[indexMemberAffected].face[j].point[k] = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                        }
                    }
                }
            }
            break;
        case Y:
            for(i = 0; i < 4; i++)
            {
                tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                model->member[indexMemberAffected].face[indexFaceAffected].point[i].y -= event.yrel * 0.005;
                dimensionResized = model->member[indexMemberAffected].face[4].point[i].y - model->member[indexMemberAffected].face[5].point[i].y;

                if(dimensionResized < 0)
                    dimensionResized *= -1;

                for(j = 0; j < 6; j++)
                {
                    for(k = 0; k < 4; k++)
                    {
                        if(model->member[indexMemberAffected].face[j].point[k].x == tmp.x && model->member[indexMemberAffected].face[j].point[k].y == tmp.y && model->member[indexMemberAffected].face[j].point[k].z == tmp.z)
                        {
                           model->member[indexMemberAffected].face[j].point[k] = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                        }
                    }
                }
            }
            break;
        case Z:
            for(i = 0; i < 4; i++)
            {
                tmp = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                model->member[indexMemberAffected].face[indexFaceAffected].point[i].z += event.xrel * 0.005;
                dimensionResized = model->member[indexMemberAffected].face[indexFaceAffected].point[i].z - model->member[indexMemberAffected].face[(indexFaceAffected + 2) % 4].point[i].z;

                if(dimensionResized < 0)
                    dimensionResized *= -1;

                for(j = 0; j < 6; j++)
                {
                    for(k = 0; k < 4; k++)
                    {
                        if(model->member[indexMemberAffected].face[j].point[k].x == tmp.x && model->member[indexMemberAffected].face[j].point[k].y == tmp.y && model->member[indexMemberAffected].face[j].point[k].z == tmp.z)
                        {
                           model->member[indexMemberAffected].face[j].point[k] = model->member[indexMemberAffected].face[indexFaceAffected].point[i];
                        }
                    }
                }
            }
            break;
    }

    printf("%lf\n", dimensionResized);
}

void renderMenuEditor(Button *button, Texture *textureText, int *weightLetter)
{
    int i;

    glPushMatrix();
    glTranslated(0, 0, -100);
    for(i = 0; i < NUMBER_BUTTON_EDITOR; i++)
    {
        renderButton(&button[i], textureText, weightLetter);
    }
    glPopMatrix();
}

int attribCube(Cube *cubeCopy, const Cube *cubeToCopy)
{
    int i, j;

    for(i = 0; i < 6; i++)
    {
        cubeCopy->IDVBO = cubeToCopy->IDVBO;
        cubeCopy->face[i].color.r = cubeToCopy->face[i].color.r;
        cubeCopy->face[i].color.v = cubeToCopy->face[i].color.v;
        cubeCopy->face[i].color.b = cubeToCopy->face[i].color.b;
        for(j = 0; j < 4; j++)
        {
            cubeCopy->face[i].point[j].x = cubeToCopy->face[i].point[j].x;
            cubeCopy->face[i].point[j].y = cubeToCopy->face[i].point[j].y;
            cubeCopy->face[i].point[j].z = cubeToCopy->face[i].point[j].z;
        }
        sprintf(cubeCopy->face[i].tex.path, "%s", cubeToCopy->face[i].tex.path);
        cubeCopy->face[i].tex.IDtex = cubeToCopy->face[i].tex.IDtex;
        cubeCopy->face[i].tex.pos.x = cubeToCopy->face[i].tex.pos.x;
        cubeCopy->face[i].tex.pos.y = cubeToCopy->face[i].tex.pos.y;
        for(j = 0; j < 2; j++)
        {
            cubeCopy->face[i].tex.posTex[j].x = cubeToCopy->face[i].tex.posTex[j].x;
            cubeCopy->face[i].tex.posTex[j].y = cubeToCopy->face[i].tex.posTex[j].y;
        }
        cubeCopy->face[i].tex.weight = cubeToCopy->face[i].tex.weight;
        cubeCopy->face[i].tex.height = cubeToCopy->face[i].tex.height;
        cubeCopy->face[i].tex.wMax = cubeToCopy->face[i].tex.wMax;
        cubeCopy->face[i].tex.hMax = cubeToCopy->face[i].tex.hMax;
        cubeCopy->face[i].tex.color.r = cubeCopy->face[i].tex.color.r;
        cubeCopy->face[i].tex.color.v = cubeCopy->face[i].tex.color.v;
        cubeCopy->face[i].tex.color.b = cubeCopy->face[i].tex.color.b;
    }

    return 1;
}
