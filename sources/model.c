#include "constants.h"
#include "model.h"
#include "animations.h"
#include "render.h"
#include "sdlglutils.h"

extern int windowWidth;
extern int windowHeight;

int initModel(Model *model)
{
    model->nbMembers = 0;
    model->tex.IDtex = 0;
    model->init = 1;
    model->saved = 0;
    model->nbAnims = 0;

    return 1;
}

int initCube(Cube *cube, int initEverything)
{
    int i, j;

    for(i = 0; i < 6; i++)
    {
        if(initEverything)
        {
            cube->face[i].color.r = i * 20 + 50;
            cube->face[i].color.v = i * 20 + 50;
            cube->face[i].color.b = i * 20 + 50;

            cube->indexParent = -1;
        }

        for(j = 0; j < 4; j++)
        {
            cube->face[i].point[j].x = cube->origin.x;
            cube->face[i].point[j].y = cube->origin.y;
            cube->face[i].point[j].z = cube->origin.z;

            if(initEverything)
            {
                cube->face[i].point[j].coordFileTexture.x = -1;
                cube->face[i].point[j].coordFileTexture.y = -1;
            }
        }
    }

    cube->face[0].point[1].y += cube->dimension.y;
    cube->face[0].point[2].x += cube->dimension.x;
    cube->face[0].point[2].y += cube->dimension.y;
    cube->face[0].point[3].x += cube->dimension.x;

    cube->face[1].point[1].y += cube->dimension.y;
    cube->face[1].point[2].y += cube->dimension.y;
    cube->face[1].point[2].z += cube->dimension.z;
    cube->face[1].point[3].z += cube->dimension.z;

    cube->face[2].point[0].z += cube->dimension.z;
    cube->face[2].point[1].x += cube->dimension.x;
    cube->face[2].point[1].z += cube->dimension.z;
    cube->face[2].point[2].x += cube->dimension.x;
    cube->face[2].point[2].y += cube->dimension.y;
    cube->face[2].point[2].z += cube->dimension.z;
    cube->face[2].point[3].y += cube->dimension.y;
    cube->face[2].point[3].z += cube->dimension.z;

    cube->face[3].point[0].x += cube->dimension.x;
    cube->face[3].point[1].x += cube->dimension.x;
    cube->face[3].point[1].y += cube->dimension.y;
    cube->face[3].point[2].x += cube->dimension.x;
    cube->face[3].point[2].y += cube->dimension.y;
    cube->face[3].point[2].z += cube->dimension.z;
    cube->face[3].point[3].x += cube->dimension.x;
    cube->face[3].point[3].z += cube->dimension.z;

    cube->face[4].point[1].x += cube->dimension.x;
    cube->face[4].point[2].x += cube->dimension.x;
    cube->face[4].point[2].z += cube->dimension.z;
    cube->face[4].point[3].z += cube->dimension.z;

    cube->face[5].point[0].y += cube->dimension.y;
    cube->face[5].point[1].x += cube->dimension.x;
    cube->face[5].point[1].y += cube->dimension.y;
    cube->face[5].point[2].x += cube->dimension.x;
    cube->face[5].point[2].y += cube->dimension.y;
    cube->face[5].point[2].z += cube->dimension.z;
    cube->face[5].point[3].z += cube->dimension.z;
    cube->face[5].point[3].y += cube->dimension.y;

    return 1;
}

int createModel(char *mainPath, char *path, Model *model)
{
    FILE *file = NULL;

    initModel(model);

    file = fopen(path, "r");

    if(file == NULL)
    {
        file = fopen(path, "w");
        if(file == NULL)
        {
            printf("Error creating Model : %s\n", path);
            return 0;
        }
    }
    else
    {
        loadModel(mainPath, path, model);
    }

    fclose(file);

    return 1;
}

int loadModel(char *mainPath, char *path, Model *model)
{
    FILE *file = fopen(path, "r");
    int i, j, k;

    if(file == NULL)
    {
        printf("Loading model failed (%s)\n", path);
        return 0;
    }
    if(model->init != 1)
    {
        printf("Trying loading model without any initialization\n");
        return 0;
    }

    fscanf(file, "nbMembers = %d\n", &model->nbMembers);
    model->tex.path[0] = 0;
    fscanf(file, "pathTexture = %s\n", model->tex.path);

    if(model->tex.path[0] != 0 && strcmp(model->tex.path, "NOTHING") != 0)
    {
        openTexture(mainPath, model);
    }
    if(model->nbMembers != 0)
    {
        for(i = 0; i < model->nbMembers; i++)
        {
            model->member[i] = malloc(sizeof(Cube));
            model->translation[i] = malloc(sizeof(Point3D));
            model->rotation[i] = malloc(sizeof(Point3D));

            if(model->member[i] == NULL || model->translation[i] == NULL || model->rotation[i] == NULL)
            {
                printf("Error allocating memory\n");
                return 0;
            }
        }

        for(i = 0; i < model->nbMembers; i++)
        {
            fscanf(file, "Member %d\n", &i);
            fscanf(file, "Index Parent : %d\n", &model->member[i]->indexParent);
            fscanf(file, "Translation : %f %f %f\n", &model->translation[i]->x, &model->translation[i]->y, &model->translation[i]->z);
            fscanf(file, "Rotation : %f %f %f\n", &model->rotation[i]->x, &model->rotation[i]->y, &model->rotation[i]->z);
            fscanf(file, "o %f %f %f\n", &model->member[i]->origin.x, &model->member[i]->origin.y, &model->member[i]->origin.z);
            fscanf(file, "d %f %f %f\n\n", &model->member[i]->dimension.x, &model->member[i]->dimension.y, &model->member[i]->dimension.z);

            initCube(model->member[i], 1);

            for(j = 0; j < 6; j++)
            {
                fscanf(file, "face %d :\n", &j);
                fscanf(file, "c %lf %lf %lf\n\n", &model->member[i]->face[j].color.r, &model->member[i]->face[j].color.v, &model->member[i]->face[j].color.b);
                for(k = 0; k < 4; k++)
                {
                    fscanf(file, "t %f %f\n", &model->member[i]->face[j].point[k].coordFileTexture.x, &model->member[i]->face[j].point[k].coordFileTexture.y);
                }

                fscanf(file, "\n");
            }
        }
    }

    fscanf(file, "nbAnims = %d\n\n", &model->nbAnims);

    if(model->nbAnims > 0)
    {
        for(i = 0; i < model->nbAnims; i++)
        {
            model->animation[i] = malloc(sizeof(Animation));

            if(model->animation[i] == NULL)
            {
                printf("Error allocating memory\n");
                return 0;
            }
        }

        for(i = 0; i < model->nbAnims; i++)
        {
            fscanf(file, "anim %d\n", &i);
            fscanf(file, "name : %s\n", model->animation[i]->animationName);
            model->animation[i]->isReversing = 0;
            fscanf(file, "nbMovements : %d\n\n", &model->animation[i]->nbMovements);

            for(j = 0; j < MEMBERS_MAX; j++)
            {
                model->animation[i]->firstValueEdited[j] = 0;
                model->animation[i]->secondValueEdited[j] = 0;
                model->animation[i]->basicValueEdited[j] = 0;
            }

            for(j = 0; j < model->animation[i]->nbMovements; j++)
            {
                fscanf(file, "index %d : %d\n", &j, &model->animation[i]->indexMemberAffected[j]);
                fscanf(file, "typeAnimation : %d\n", &model->animation[i]->typeAnimation[j]);
                fscanf(file, "axisAnimated : %d\n", &model->animation[i]->axisAnimated[j]);
                fscanf(file, "minimalValue : %f\n", &model->animation[i]->minimalValue[j]);
                fscanf(file, "maximalValue : %f\n", &model->animation[i]->maximalValue[j]);
                fscanf(file, "basicValue : %f\n", &model->animation[i]->basicValue[j]);
                fscanf(file, "phase : %d\n", &model->animation[i]->initialPhase[j]);
                fscanf(file, "period : %d\n\n", &model->animation[i]->period[j]);

                model->animation[i]->currentPhase[j] = model->animation[i]->initialPhase[j];
                model->animation[i]->phaseChanged[j] = 0;
                model->animation[i]->firstValueEdited[j] = 1;
                model->animation[i]->secondValueEdited[j] = 1;
                model->animation[i]->basicValueEdited[j] = 1;
            }

            model->animation[i]->lastUpdate = -1;
        }
    }

    fclose(file);

    return 1;
}

int saveModel(char *path, Model *model)
{
    FILE *file = NULL;
    int i, j, k;

    file = fopen(path, "w+");

    if(file == NULL)
    {
        printf("Saving model failed (%s)\n", path);
        return 0;
    }

    fprintf(file, "nbMembers = %d\n", model->nbMembers);

    if(model->tex.IDtex != 0)
        fprintf(file, "pathTexture = %s\n", model->tex.path);
    else
        fprintf(file, "pathTexture = NOTHING\n");

    for(i = 0; i < model->nbMembers; i++)
    {
        fprintf(file, "Member %d\n", i);
        fprintf(file, "Index Parent : %d\n", model->member[i]->indexParent);
        fprintf(file, "Translation : %f %f %f\n", model->translation[i]->x, model->translation[i]->y, model->translation[i]->z);
        fprintf(file, "Rotation : %f %f %f\n", model->rotation[i]->x, model->rotation[i]->y, model->rotation[i]->z);
        fprintf(file, "o %f %f %f\n", model->member[i]->origin.x, model->member[i]->origin.y, model->member[i]->origin.z);
        fprintf(file, "d %f %f %f\n\n", model->member[i]->dimension.x, model->member[i]->dimension.y, model->member[i]->dimension.z);

        for(j = 0; j < 6; j++)
        {
            fprintf(file, "face %d :\n", j);
            fprintf(file, "c %lf %lf %lf\n\n", model->member[i]->face[j].color.r, model->member[i]->face[j].color.v, model->member[i]->face[j].color.b);
            for(k = 0; k < 4; k++)
            {
                fprintf(file, "t %lf %lf\n", model->member[i]->face[j].point[k].coordFileTexture.x, model->member[i]->face[j].point[k].coordFileTexture.y);
            }
            fprintf(file, "\n");
        }
    }

    fprintf(file, "nbAnims = %d\n\n", model->nbAnims);

    for(i = 0; i < model->nbAnims; i++)
    {
        fprintf(file, "anim %d\n", i);
        fprintf(file, "name : %s\n", model->animation[i]->animationName);
        fprintf(file, "nbMovements : %d\n\n", model->animation[i]->nbMovements);

        for(j = 0; j < model->animation[i]->nbMovements; j++)
        {
            fprintf(file, "index %d : %d\n", j, model->animation[i]->indexMemberAffected[j]);
            fprintf(file, "typeAnimation : %d\n", model->animation[i]->typeAnimation[j]);
            fprintf(file, "axisAnimated : %d\n", model->animation[i]->axisAnimated[j]);
            fprintf(file, "minimalValue : %f\n", model->animation[i]->minimalValue[j]);
            fprintf(file, "maximalValue : %f\n", model->animation[i]->maximalValue[j]);
            fprintf(file, "basicValue : %f\n", model->animation[i]->basicValue[j]);
            fprintf(file, "phase : %d\n", model->animation[i]->initialPhase[j]);
            fprintf(file, "period : %d\n\n", model->animation[i]->period[j]);
        }
    }

    fclose(file);

    return 1;
}

int freeModel(Model *model)
{
    int i;

    for(i = 0; i < model->nbMembers; i++)
    {
        free(model->member[i]);
        free(model->translation[i]);
        free(model->rotation[i]);
    }

    for(i = 0; i < model->nbAnims; i++)
    {
        free(model->animation[i]);
    }

    initModel(model);

    return 1;
}

int renderModel(Model *model, int mode)
{
    int i, j, k, l;
    Face faceBuff;
    int basicIndexW = 0, basicIndexL = 0;
    double width = -1, length = -1;

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, model->tex.IDtex);

    for(i = 0; i < model->nbMembers; i++)//Cubes
    {
        glPushMatrix();

        glTranslatef(model->translation[i]->x, model->translation[i]->y, model->translation[i]->z);

        glRotatef(model->rotation[i]->x, 1, 0, 0);
        glRotatef(model->rotation[i]->y, 0, 1, 0);
        glRotatef(model->rotation[i]->z, 0, 0, 1);

        for(j = 0; j < 6; j++)//Faces
        {
            if(mode == RENDER_MODE)
            {
                glEnable(GL_BLEND);
                glColor3ub(model->member[i]->face[j].color.r, model->member[i]->face[j].color.v, model->member[i]->face[j].color.b);
            }
            else if(mode == COLLISION_MODE_EDITOR)
            {
                glColor3ub(i + 10, j + 10, j + 10);//To find indexes in the collision with glReadPixels();
            }
            else if(mode == COLLISION_MODE_ANIMATOR)
            {
                if(model->member[i]->face[j].point[0].x == model->member[i]->face[j].point[2].x)
                {
                    width = model->member[i]->face[j].point[0].z - model->member[i]->face[j].point[2].z;
                    length = model->member[i]->face[j].point[0].y - model->member[i]->face[j].point[2].y;
                }
                else if(model->member[i]->face[j].point[0].y == model->member[i]->face[j].point[2].y)
                {
                    width = model->member[i]->face[j].point[0].x - model->member[i]->face[j].point[2].x;
                    length = model->member[i]->face[j].point[0].z - model->member[i]->face[j].point[2].z;
                }
                else if(model->member[i]->face[j].point[0].z == model->member[i]->face[j].point[2].z)
                {
                    width = model->member[i]->face[j].point[0].x - model->member[i]->face[j].point[2].x;
                    length = model->member[i]->face[j].point[0].y - model->member[i]->face[j].point[2].y;
                }

                if(width < 0)
                    width *= -1;
                if(length < 0)
                    length *= -1;

                for(k = 0; k < PRECISION_COLLISION; k++)
                {
                    for(l = 0; l < PRECISION_COLLISION; l++)
                    {
                        glColor3ub(j + 10, k + 10, l + 10);

                        faceBuff.point[0] = model->member[i]->face[j].point[0];
                        faceBuff.point[1] = model->member[i]->face[j].point[1];
                        faceBuff.point[2] = model->member[i]->face[j].point[2];
                        faceBuff.point[3] = model->member[i]->face[j].point[3];

                        if(model->member[i]->face[j].point[0].x == model->member[i]->face[j].point[2].x)
                        {
                            if(faceBuff.point[0].z > faceBuff.point[2].z)
                                basicIndexW = 2;

                            if(faceBuff.point[0].y > faceBuff.point[2].y)
                                basicIndexL = 2;

                            faceBuff.point[0].z = model->member[i]->face[j].point[basicIndexW].z + (width / PRECISION_COLLISION) * k;
                            faceBuff.point[0].y = model->member[i]->face[j].point[basicIndexL].y + (length / PRECISION_COLLISION) * l;

                            faceBuff.point[1].z = faceBuff.point[0].z;
                            faceBuff.point[1].y = model->member[i]->face[j].point[basicIndexL].y + (length / PRECISION_COLLISION) * (l + 1);

                            faceBuff.point[2].z = model->member[i]->face[j].point[basicIndexW].z + (width / PRECISION_COLLISION) * (k + 1);
                            faceBuff.point[2].y = faceBuff.point[1].y;

                            faceBuff.point[3].z = faceBuff.point[2].z;
                            faceBuff.point[3].y = faceBuff.point[0].y;
                        }
                        else if(model->member[i]->face[j].point[0].y == model->member[i]->face[j].point[2].y)
                        {
                            if(faceBuff.point[0].x > faceBuff.point[2].x)
                                basicIndexW = 2;

                            if(faceBuff.point[0].z > faceBuff.point[2].z)
                                basicIndexL = 2;

                            faceBuff.point[0].x = model->member[i]->face[j].point[basicIndexW].x + (width / PRECISION_COLLISION) * k;
                            faceBuff.point[0].z = model->member[i]->face[j].point[basicIndexL].z + (length / PRECISION_COLLISION) * l;

                            faceBuff.point[1].x = faceBuff.point[0].x;
                            faceBuff.point[1].z = model->member[i]->face[j].point[basicIndexL].z + (length / PRECISION_COLLISION) * (l + 1);

                            faceBuff.point[2].x = model->member[i]->face[j].point[basicIndexW].x + (width / PRECISION_COLLISION) * (k + 1);
                            faceBuff.point[2].z = faceBuff.point[1].z;

                            faceBuff.point[3].x = faceBuff.point[2].x;
                            faceBuff.point[3].z = faceBuff.point[0].z;
                        }
                        else if(model->member[i]->face[j].point[0].z == model->member[i]->face[j].point[2].z)
                        {
                            if(faceBuff.point[0].x > faceBuff.point[2].x)
                                basicIndexW = 2;

                            if(faceBuff.point[0].y > faceBuff.point[2].y)
                                basicIndexL = 2;

                            faceBuff.point[0].x = model->member[i]->face[j].point[basicIndexW].x + (width / PRECISION_COLLISION) * k;
                            faceBuff.point[0].y = model->member[i]->face[j].point[basicIndexL].y + (length / PRECISION_COLLISION) * l;

                            faceBuff.point[1].x = faceBuff.point[0].x;
                            faceBuff.point[1].y = model->member[i]->face[j].point[basicIndexL].y + (length / PRECISION_COLLISION) * (l + 1);

                            faceBuff.point[2].x = model->member[i]->face[j].point[basicIndexW].x + (width / PRECISION_COLLISION) * (k + 1);
                            faceBuff.point[2].y = faceBuff.point[1].y;

                            faceBuff.point[3].x = faceBuff.point[2].x;
                            faceBuff.point[3].y = faceBuff.point[0].y;
                        }

                        drawFace(&faceBuff, mode);

                        basicIndexW = 0;
                        basicIndexL = 0;
                    }
                }
            }

            if(mode != COLLISION_MODE_ANIMATOR)
            {
                drawFace(&model->member[i]->face[j], mode);
            }
            glDisable(GL_BLEND);
        }
        glPopMatrix();
    }

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

    return 1;
}

void renderMember(Model *model, int indexMemberToRender, int mode)
{
    int i, k, l;
    int basicIndexW = 0, basicIndexL = 0;
    double width = -1, length = -1;
    Face faceBuff;

    glPushMatrix();

    glTranslatef(model->translation[indexMemberToRender]->x, model->translation[indexMemberToRender]->y, model->translation[indexMemberToRender]->z);

    glRotatef(model->rotation[indexMemberToRender]->x, 1, 0, 0);
    glRotatef(model->rotation[indexMemberToRender]->y, 0, 1, 0);
    glRotatef(model->rotation[indexMemberToRender]->z, 0, 0, 1);

    glBindTexture(GL_TEXTURE_2D, model->tex.IDtex);

    for(i = 0; i < 6; i++)
    {
        if(mode == RENDER_MODE)
        {
            glEnable(GL_BLEND);
            glColor3ub(model->member[indexMemberToRender]->face[i].color.r, model->member[indexMemberToRender]->face[i].color.v, model->member[indexMemberToRender]->face[i].color.b);
        }
        else if(mode == COLLISION_MODE_EDITOR)
        {
            glColor3ub(indexMemberToRender + 10, i + 10, i + 10);//To find indexes in the collision with glReadPixels();
        }
        else if(mode == COLLISION_MODE_ANIMATOR)
        {
            if(model->member[indexMemberToRender]->face[i].point[0].x == model->member[indexMemberToRender]->face[i].point[2].x)
            {
                width = model->member[indexMemberToRender]->face[i].point[0].z - model->member[indexMemberToRender]->face[i].point[2].z;
                length = model->member[indexMemberToRender]->face[i].point[0].y - model->member[indexMemberToRender]->face[i].point[2].y;
            }
            else if(model->member[indexMemberToRender]->face[i].point[0].y == model->member[indexMemberToRender]->face[i].point[2].y)
            {
                width = model->member[indexMemberToRender]->face[i].point[0].x - model->member[indexMemberToRender]->face[i].point[2].x;
                length = model->member[indexMemberToRender]->face[i].point[0].z - model->member[indexMemberToRender]->face[i].point[2].z;
            }
            else if(model->member[indexMemberToRender]->face[i].point[0].z == model->member[indexMemberToRender]->face[i].point[2].z)
            {
                width = model->member[indexMemberToRender]->face[i].point[0].x - model->member[indexMemberToRender]->face[i].point[2].x;
                length = model->member[indexMemberToRender]->face[i].point[0].y - model->member[indexMemberToRender]->face[i].point[2].y;
            }

            if(width < 0)
                width *= -1;
            if(length < 0)
                length *= -1;

            for(k = 0; k < PRECISION_COLLISION; k++)
            {
                for(l = 0; l < PRECISION_COLLISION; l++)
                {
                    glColor3ub(i + 10, k + 10, l + 10);

                    faceBuff.point[0] = model->member[indexMemberToRender]->face[i].point[0];
                    faceBuff.point[1] = model->member[indexMemberToRender]->face[i].point[1];
                    faceBuff.point[2] = model->member[indexMemberToRender]->face[i].point[2];
                    faceBuff.point[3] = model->member[indexMemberToRender]->face[i].point[3];

                    if(model->member[indexMemberToRender]->face[i].point[0].x == model->member[indexMemberToRender]->face[i].point[2].x)
                    {
                        if(faceBuff.point[0].z > faceBuff.point[2].z)
                            basicIndexW = 2;

                        if(faceBuff.point[0].y > faceBuff.point[2].y)
                            basicIndexL = 2;

                        faceBuff.point[0].z = model->member[indexMemberToRender]->face[i].point[basicIndexW].z + (width / PRECISION_COLLISION) * k;
                        faceBuff.point[0].y = model->member[indexMemberToRender]->face[i].point[basicIndexL].y + (length / PRECISION_COLLISION) * l;

                        faceBuff.point[1].z = faceBuff.point[0].z;
                        faceBuff.point[1].y = model->member[indexMemberToRender]->face[i].point[basicIndexL].y + (length / PRECISION_COLLISION) * (l + 1);

                        faceBuff.point[2].z = model->member[indexMemberToRender]->face[i].point[basicIndexW].z + (width / PRECISION_COLLISION) * (k + 1);
                        faceBuff.point[2].y = faceBuff.point[1].y;

                        faceBuff.point[3].z = faceBuff.point[2].z;
                        faceBuff.point[3].y = faceBuff.point[0].y;
                    }
                    else if(model->member[indexMemberToRender]->face[i].point[0].y == model->member[indexMemberToRender]->face[i].point[2].y)
                    {
                        if(faceBuff.point[0].x > faceBuff.point[2].x)
                            basicIndexW = 2;

                        if(faceBuff.point[0].z > faceBuff.point[2].z)
                            basicIndexL = 2;

                        faceBuff.point[0].x = model->member[indexMemberToRender]->face[i].point[basicIndexW].x + (width / PRECISION_COLLISION) * k;
                        faceBuff.point[0].z = model->member[indexMemberToRender]->face[i].point[basicIndexL].z + (length / PRECISION_COLLISION) * l;

                        faceBuff.point[1].x = faceBuff.point[0].x;
                        faceBuff.point[1].z = model->member[indexMemberToRender]->face[i].point[basicIndexL].z + (length / PRECISION_COLLISION) * (l + 1);

                        faceBuff.point[2].x = model->member[indexMemberToRender]->face[i].point[basicIndexW].x + (width / PRECISION_COLLISION) * (k + 1);
                        faceBuff.point[2].z = faceBuff.point[1].z;

                        faceBuff.point[3].x = faceBuff.point[2].x;
                        faceBuff.point[3].z = faceBuff.point[0].z;
                    }
                    else if(model->member[indexMemberToRender]->face[i].point[0].z == model->member[indexMemberToRender]->face[i].point[2].z)
                    {
                        if(faceBuff.point[0].x > faceBuff.point[2].x)
                            basicIndexW = 2;

                        if(faceBuff.point[0].y > faceBuff.point[2].y)
                            basicIndexL = 2;

                        faceBuff.point[0].x = model->member[indexMemberToRender]->face[i].point[basicIndexW].x + (width / PRECISION_COLLISION) * k;
                        faceBuff.point[0].y = model->member[indexMemberToRender]->face[i].point[basicIndexL].y + (length / PRECISION_COLLISION) * l;

                        faceBuff.point[1].x = faceBuff.point[0].x;
                        faceBuff.point[1].y = model->member[indexMemberToRender]->face[i].point[basicIndexL].y + (length / PRECISION_COLLISION) * (l + 1);

                        faceBuff.point[2].x = model->member[indexMemberToRender]->face[i].point[basicIndexW].x + (width / PRECISION_COLLISION) * (k + 1);
                        faceBuff.point[2].y = faceBuff.point[1].y;

                        faceBuff.point[3].x = faceBuff.point[2].x;
                        faceBuff.point[3].y = faceBuff.point[0].y;
                    }

                    drawFace(&faceBuff, mode);

                    basicIndexW = 0;
                    basicIndexL = 0;
                }
            }
        }

        if(mode != COLLISION_MODE_ANIMATOR)
        {
            drawFace(&model->member[indexMemberToRender]->face[i], mode);
        }
        glDisable(GL_BLEND);
    }

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);
}

int openTexture(char *mainPath, Model *model)
{
    char buff[SIZE_PATH_MAX] = "";
    char buff2[SIZE_PATH_MAX] = "";

    sprintf(buff, "%s", mainPath);

    strcat(buff, model->tex.path);

    strcpy(buff2, model->tex.path);
    strcpy(model->tex.path, buff);
    loadTexture(&model->tex);
    strcpy(model->tex.path, buff2);

    model->tex.posTex[0].x = 0;
    model->tex.posTex[0].y = 1;
    model->tex.posTex[1].x = 1;
    model->tex.posTex[1].y = 0;
    model->tex.color.r = 255;
    model->tex.color.v = 255;
    model->tex.color.b = 255;
    model->tex.height = model->tex.hMax;
    model->tex.weight = model->tex.wMax;
    model->tex.pos.x = windowWidth - model->tex.weight - 10;
    model->tex.pos.y = (windowHeight - model->tex.height) / 2;

    return 1;
}

int animateModel(Model *model, char *nameAnimation)
{
    int i;
    int indexAnimation = -1;
    int dataReturned = 2;
    int actualTime = SDL_GetTicks();
    float *dataToAnimate = NULL;
    float dataNotAnimated = 0;

    indexAnimation = searchAnimation(model, nameAnimation);

    if(indexAnimation == -1)
    {
        printf("Animation not found : %s\n", nameAnimation);
        return 0;
    }

    if(model->animation[indexAnimation]->lastUpdate == -1)
    {
        model->animation[indexAnimation]->lastUpdate = SDL_GetTicks();
    }

    for(i = 0; i < model->animation[indexAnimation]->nbMovements; i++)
    {
        if(model->animation[indexAnimation]->typeAnimation[i] == ROTATION_ANIMATION)
        {
            switch(model->animation[indexAnimation]->axisAnimated[i])
            {
                case X:
                    dataToAnimate = &model->rotation[model->animation[indexAnimation]->indexMemberAffected[i]]->x;
                    break;
                case Y:
                    dataToAnimate = &model->rotation[model->animation[indexAnimation]->indexMemberAffected[i]]->y;
                    break;
                case Z:
                    dataToAnimate = &model->rotation[model->animation[indexAnimation]->indexMemberAffected[i]]->z;
                    break;
            }
        }
        else if(model->animation[indexAnimation]->typeAnimation[i] == TRANSLATION_ANIMATION)
        {
            switch(model->animation[indexAnimation]->axisAnimated[i])
            {
                case X:
                    dataToAnimate = &model->translation[model->animation[indexAnimation]->indexMemberAffected[i]]->x;
                    break;
                case Y:
                    dataToAnimate = &model->translation[model->animation[indexAnimation]->indexMemberAffected[i]]->y;
                    break;
                case Z:
                    dataToAnimate = &model->translation[model->animation[indexAnimation]->indexMemberAffected[i]]->z;
                    break;
            }
        }
        dataNotAnimated = (*dataToAnimate);

        if(model->animation[indexAnimation]->currentPhase[i] == INCREASING)
        {
            if(model->animation[indexAnimation]->maximalValue[i] != model->animation[indexAnimation]->minimalValue[i])
            {
                (*dataToAnimate) += ((actualTime - model->animation[indexAnimation]->lastUpdate) * (model->animation[indexAnimation]->maximalValue[i] - model->animation[indexAnimation]->minimalValue[i])) / model->animation[indexAnimation]->period[i];
            }
            else
            {
                (*dataToAnimate) += (model->animation[indexAnimation]->basicValue[i] - model->animation[indexAnimation]->maximalValue[i]) / (float)(actualTime - model->animation[indexAnimation]->lastUpdate);
            }

            if((*dataToAnimate) >= model->animation[indexAnimation]->maximalValue[i] && model->animation[indexAnimation]->isReversing == 0)
            {
                model->animation[indexAnimation]->currentPhase[i] = DECREASING;
                (*dataToAnimate) = model->animation[indexAnimation]->maximalValue[i];
            }
            if(model->animation[indexAnimation]->isReversing == 1)
            {
                if(((*dataToAnimate) < model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->currentPhase[i] == INCREASING) || model->animation[indexAnimation]->phaseChanged[i] == 0)
                {
                    dataReturned = 1;
                }
                if((*dataToAnimate) > model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->currentPhase[i] == INCREASING && model->animation[indexAnimation]->phaseChanged[i] == 1)
                {
                    (*dataToAnimate) = model->animation[indexAnimation]->basicValue[i];
                }
                if((*dataToAnimate) > model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->phaseChanged[i] == 0)
                {
                    model->animation[indexAnimation]->currentPhase[i] = DECREASING;
                    model->animation[indexAnimation]->phaseChanged[i] = 1;
                }
                else if((*dataToAnimate) < model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->phaseChanged[i] == 0)
                {
                    model->animation[indexAnimation]->currentPhase[i] = INCREASING;
                    model->animation[indexAnimation]->phaseChanged[i] = 1;
                }
            }
        }

        else if(model->animation[indexAnimation]->currentPhase[i] == DECREASING)
        {
            if(model->animation[indexAnimation]->maximalValue[i] != model->animation[indexAnimation]->minimalValue[i])
            {
                (*dataToAnimate) -= ((actualTime - model->animation[indexAnimation]->lastUpdate) * (model->animation[indexAnimation]->maximalValue[i] - model->animation[indexAnimation]->minimalValue[i])) / model->animation[indexAnimation]->period[i];
            }
            else
            {
                (*dataToAnimate) -= (model->animation[indexAnimation]->basicValue[i] - model->animation[indexAnimation]->maximalValue[i]) / (float)(actualTime - model->animation[indexAnimation]->lastUpdate);
            }

            if((*dataToAnimate) <= model->animation[indexAnimation]->minimalValue[i] && model->animation[indexAnimation]->isReversing == 0)
            {
                model->animation[indexAnimation]->currentPhase[i] = INCREASING;
                (*dataToAnimate) = model->animation[indexAnimation]->minimalValue[i];
            }
            if(model->animation[indexAnimation]->isReversing == 1)
            {
                if(((*dataToAnimate) > model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->currentPhase[i] == DECREASING) || model->animation[indexAnimation]->phaseChanged[i] == 0)
                {
                    dataReturned = 1;
                }
                if((*dataToAnimate) < model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->currentPhase[i] == DECREASING && model->animation[indexAnimation]->phaseChanged[i] == 1)
                {
                    (*dataToAnimate) = model->animation[indexAnimation]->basicValue[i];
                }
                if((*dataToAnimate) > model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->phaseChanged[i] == 0)
                {
                    model->animation[indexAnimation]->currentPhase[i] = DECREASING;
                    model->animation[indexAnimation]->phaseChanged[i] = 1;
                }
                else if((*dataToAnimate) < model->animation[indexAnimation]->basicValue[i] && model->animation[indexAnimation]->phaseChanged[i] == 0)
                {
                    model->animation[indexAnimation]->currentPhase[i] = INCREASING;
                    model->animation[indexAnimation]->phaseChanged[i] = 1;
                }
            }
        }

        animateChildren(model, indexAnimation, i, (*dataToAnimate), dataNotAnimated);
    }

    model->animation[indexAnimation]->lastUpdate = SDL_GetTicks();

    if(model->animation[indexAnimation]->isReversing == 0)
    {
        dataReturned = 1;
    }

    if(dataReturned == 2)
    {
        model->animation[indexAnimation]->isReversing = 0;
        model->animation[indexAnimation]->lastUpdate = -1;

        for(i = 0; i < model->animation[indexAnimation]->nbMovements; i++)
        {
            model->animation[indexAnimation]->phaseChanged[i] = 0;
            model->animation[indexAnimation]->currentPhase[i] = model->animation[indexAnimation]->initialPhase[i];
        }
    }

    return dataReturned;
}

int stopAnimation(Model *model, char *nameAnimation)
{
    int indexAnimation = -1;

    indexAnimation = searchAnimation(model, nameAnimation);

    if(indexAnimation == -1)
    {
        printf("Animation not found : %s\n", nameAnimation);
        return 0;
    }

    model->animation[indexAnimation]->lastUpdate = -1;
    model->animation[indexAnimation]->isReversing = 1;

    return 1;
}

int searchAnimation(Model *model, char *animationName)
{
    int i;
    int indexAnimation = -1;

    for(i = 0; i < model->nbAnims; i++)
    {
        if(strcmp(animationName, model->animation[i]->animationName) == 0)
        {
            indexAnimation = i;
        }
    }

    return indexAnimation;
}

int resetModelAnimation(Model *model, char *animationName)
{
    int indexAnimation = -1;
    float (*dataToReset) = NULL;
    int i;

    indexAnimation = searchAnimation(model, animationName);

    if(indexAnimation == -1)
    {
        printf("Animation not found : %s\n", animationName);
        return 0;
    }

    for(i = 0; i < model->animation[indexAnimation]->nbMovements; i++)
    {
        if(model->animation[indexAnimation]->typeAnimation[i] == ROTATION_ANIMATION)
        {
            switch(model->animation[indexAnimation]->axisAnimated[i])
            {
                case X:
                    dataToReset = &model->rotation[model->animation[indexAnimation]->indexMemberAffected[i]]->x;
                    break;
                case Y:
                    dataToReset = &model->rotation[model->animation[indexAnimation]->indexMemberAffected[i]]->y;
                    break;
                case Z:
                    dataToReset = &model->rotation[model->animation[indexAnimation]->indexMemberAffected[i]]->z;
                    break;
            }
        }
        else if(model->animation[indexAnimation]->typeAnimation[i] == TRANSLATION_ANIMATION)
        {
            switch(model->animation[indexAnimation]->axisAnimated[i])
            {
                case X:
                    dataToReset = &model->translation[model->animation[indexAnimation]->indexMemberAffected[i]]->x;
                    break;
                case Y:
                    dataToReset = &model->translation[model->animation[indexAnimation]->indexMemberAffected[i]]->y;
                    break;
                case Z:
                    dataToReset = &model->translation[model->animation[indexAnimation]->indexMemberAffected[i]]->z;
                    break;
            }
        }

        (*dataToReset )= model->animation[indexAnimation]->basicValue[i];
    }

    return 1;
}

void calculateChildData(Model *model)
{
    int i;
    int indexChild, indexParent;

    for(i = 0; i < model->nbMembers; i++)
    {
        indexParent = model->member[i]->indexParent;

        if(indexParent != -1)
        {
            indexChild = i;

            model->member[indexChild]->realOrigin.x = model->member[indexChild]->face[0].point[0].x - model->member[indexChild]->origin.x + model->translation[indexChild]->x;
            model->member[indexChild]->realOrigin.y = model->member[indexChild]->face[0].point[0].y - model->member[indexChild]->origin.y + model->translation[indexChild]->y;
            model->member[indexChild]->realOrigin.z = model->member[indexChild]->face[0].point[0].z - model->member[indexChild]->origin.z + model->translation[indexChild]->z;

            model->member[indexParent]->realOrigin.x = model->member[indexParent]->face[0].point[0].x - model->member[indexParent]->origin.x + model->translation[indexParent]->x;
            model->member[indexParent]->realOrigin.y = model->member[indexParent]->face[0].point[0].y - model->member[indexParent]->origin.y + model->translation[indexParent]->y;
            model->member[indexParent]->realOrigin.z = model->member[indexParent]->face[0].point[0].z - model->member[indexParent]->origin.z + model->translation[indexParent]->z;


            model->member[indexChild]->distanceParent.x = sqrt(pow(model->member[indexChild]->realOrigin.y - model->member[indexParent]->realOrigin.y, 2)
                                                            + pow(model->member[indexChild]->realOrigin.z - model->member[indexParent]->realOrigin.z, 2));

            model->member[indexChild]->distanceParent.z = sqrt(pow(model->member[indexChild]->realOrigin.x - model->member[indexParent]->realOrigin.x, 2)
                                                            + pow(model->member[indexChild]->realOrigin.y - model->member[indexParent]->realOrigin.y, 2));

            model->member[indexChild]->distanceParent.y = sqrt(pow(model->member[indexChild]->realOrigin.x - model->member[indexParent]->realOrigin.x, 2)
                                                            + pow(model->member[indexChild]->realOrigin.z - model->member[indexParent]->realOrigin.z, 2));


            model->member[indexChild]->angleParent.x = acosf((model->member[indexChild]->realOrigin.y - model->member[indexParent]->realOrigin.y)
                                                            / model->member[indexChild]->distanceParent.x);

            model->member[indexChild]->angleParent.z = acosf((model->member[indexChild]->realOrigin.x - model->member[indexParent]->realOrigin.x)
                                                            / model->member[indexChild]->distanceParent.x);

            model->member[indexChild]->angleParent.y = acosf((model->member[indexChild]->realOrigin.z - model->member[indexParent]->realOrigin.z)
                                                            / model->member[indexChild]->distanceParent.x);
        }
    }
}

void animateChildren(Model *model, int indexAnimation, int indexMovement, float dataAnimated, float lastData)
{
    int i;
    int indexParent = model->animation[indexAnimation]->indexMemberAffected[indexMovement];

    for(i = 0; i < model->nbMembers; i++)
    {
        if(model->member[i]->indexParent == indexParent)
        {
            if(model->animation[indexAnimation]->typeAnimation[indexMovement] == TRANSLATION_ANIMATION)
            {
                switch(model->animation[indexAnimation]->axisAnimated[indexMovement])
                {
                    case X:
                        model->translation[i]->x += dataAnimated - lastData;
                        break;
                    case Y:
                        model->translation[i]->y += dataAnimated - lastData;
                        break;
                    case Z:
                        model->translation[i]->z += dataAnimated - lastData;
                        break;
                }
            }
            else if(model->animation[indexAnimation]->typeAnimation[indexMovement] == ROTATION_ANIMATION)
            {
                switch(model->animation[indexAnimation]->axisAnimated[indexMovement])
                {
                    case X:
                        if(model->member[i]->distanceParent.x != 0)
                        {
                            model->translation[i]->y = model->member[i]->distanceParent.x * cos(M_PI * dataAnimated / 180 + model->member[i]->angleParent.x)
                                                        + model->member[indexParent]->realOrigin.y;
                            model->translation[i]->z = model->member[i]->distanceParent.x * sin(M_PI * dataAnimated / 180 + model->member[i]->angleParent.x)
                                                        + model->member[indexParent]->realOrigin.z;
                        }

                        model->rotation[i]->x += dataAnimated - lastData;

                        break;
                    case Y:
                        if(model->member[i]->distanceParent.y != 0)
                        {
                            model->translation[i]->x = model->member[i]->distanceParent.y * sin(M_PI * dataAnimated / 180 + model->member[i]->angleParent.y)
                                                        + model->member[indexParent]->realOrigin.x;
                            model->translation[i]->z = model->member[i]->distanceParent.y * cos(M_PI * dataAnimated / 180 + model->member[i]->angleParent.y)
                                                        + model->member[indexParent]->realOrigin.z;
                        }

                        model->rotation[i]->y += dataAnimated - lastData;
                        break;
                    case Z:
                        if(model->member[i]->distanceParent.z != 0)
                        {
                            model->translation[i]->x = model->member[i]->distanceParent.z * sin(M_PI * dataAnimated / 180 + model->member[i]->angleParent.z)
                                                        + model->member[indexParent]->realOrigin.x;
                            model->translation[i]->y = model->member[i]->distanceParent.z * cos(M_PI * dataAnimated / 180 + model->member[i]->angleParent.z)
                                                        + model->member[indexParent]->realOrigin.y;
                        }

                        model->rotation[i]->z += dataAnimated - lastData;
                        break;
                }
            }
        }
    }
}
