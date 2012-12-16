#include "constants.h"
#include "model.h"
#include "render.h"
#include "sdlglutils.h"

int initModel(Model *model)
{
    model->nbMembers = 0;
    model->member = NULL;
    model->translation = NULL;
    model->tex.IDtex = 0;
    model->init = 1;
    model->saved = 0;

    return 1;
}

int createModel(char *path, Model *model)
{
    char buff[SIZE_PATH_MAX] = "models/";
    FILE *file = NULL;

    initModel(model);

    if(strstr(path, buff) == NULL)
    {
        strcat(buff, path);
        strcpy(path, buff);
    }
    if(strstr(path, ".mclmdl") == NULL)
    {
        strcat(path, ".mclmdl");
    }

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
        loadModel(path, model);
    }

    fclose(file);

    return 1;
}

int loadModel(char *path, Model *model)
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
        openTexture(model);
    }
    if(model->nbMembers != 0)
    {
        model->member = malloc(model->nbMembers * sizeof(Cube));
        model->translation = malloc(model->nbMembers * sizeof(Point3D));

        if(model->member == NULL || model->translation == NULL)
        {
            printf("Error allocating memory\n");
            return 0;
        }

        for(i = 0; i < model->nbMembers; i++)
        {
            fscanf(file, "Member %d\n", &i);
            fscanf(file, "Translation : %f %f %f\n", &model->translation[i].x, &model->translation[i].y, &model->translation[i].z);
            for(j = 0; j < 6; j++)
            {
                fscanf(file, "face %d :\n", &j);
                fscanf(file, "c %lf %lf %lf\n\n", &model->member[i].face[j].color.r, &model->member[i].face[j].color.v, &model->member[i].face[j].color.b);
                for(k = 0; k < 4; k++)
                {
                    fscanf(file, "v %f, %f, %f\n", &model->member[i].face[j].point[k].x, &model->member[i].face[j].point[k].y, &model->member[i].face[j].point[k].z);
                    fscanf(file, "t %lf %lf\n", &model->member[i].face[j].point[k].coordFileTexture.x, &model->member[i].face[j].point[k].coordFileTexture.y);
                }
            }
        }
    }

    fclose(file);

    return 1;
}

int saveModel(char *path, Model *model)
{
    char buff[SIZE_PATH_MAX] = "models/";
    FILE *file = NULL;
    int i, j, k;

    if(strstr(path, buff) == NULL)
    {
        strcat(buff, path);
        strcpy(path, buff);
    }
    if(strstr(path, ".mclmdl") == NULL)
    {
        strcat(path, ".mclmdl");
    }

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
        fprintf(file, "Translation : %f %f %f\n", model->translation[i].x, model->translation[i].y, model->translation[i].z);
        for(j = 0; j < 6; j++)
        {
            fprintf(file, "face %d :\n", j);
            fprintf(file, "c %lf %lf %lf\n\n", model->member[i].face[j].color.r, model->member[i].face[j].color.v, model->member[i].face[j].color.b);
            for(k = 0; k < 4; k++)
            {
                fprintf(file, "v %f, %f, %f\n", model->member[i].face[j].point[k].x, model->member[i].face[j].point[k].y, model->member[i].face[j].point[k].z);
                fprintf(file, "t %lf %lf\n", model->member[i].face[j].point[k].coordFileTexture.x, model->member[i].face[j].point[k].coordFileTexture.y);
            }
        }
    }

    fclose(file);

    return 1;
}

int freeModel(Model *model)
{
    free(model->member);
    free(model->translation);

    model->nbMembers = 0;
    model->tex.IDtex = 0;

    return 1;
}

int renderModel(Model *model, int mode)
{
    int i, j;

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, model->tex.IDtex);

    for(i = 0; i < model->nbMembers; i++)
    {
        glPushMatrix();
        glTranslated(model->translation[i].x, model->translation[i].y, model->translation[i].z);

        for(j = 0; j < 6; j++)
        {
            if(mode == RENDER_MODE)
            {
                glEnable(GL_BLEND);
                glColor3ub(model->member[i].face[j].color.r, model->member[i].face[j].color.v, model->member[i].face[j].color.b);
            }
            else if(mode == COLLISION_MODE)
            {
                glColor3ub(i + 10, j + 10, j + 10);//To find indexes in the collision with glReadPixels();
            }

            drawFace(&model->member[i].face[j], mode);
            glDisable(GL_BLEND);
        }
        glPopMatrix();
    }

    glPopMatrix();

    return 1;
}

int openTexture(Model *model)
{
    char buff[SIZE_PATH_MAX] = "textures/";

    if(strstr(model->tex.path, buff) == NULL)
    {
        strcat(buff, model->tex.path);
        strcpy(model->tex.path, buff);
    }

    loadTexture(&model->tex);

    model->tex.posTex[0].x = 0;
    model->tex.posTex[0].y = 1;
    model->tex.posTex[1].x = 1;
    model->tex.posTex[1].y = 0;
    model->tex.color.r = 255;
    model->tex.color.v = 255;
    model->tex.color.b = 255;
    model->tex.height = model->tex.hMax;
    model->tex.weight = model->tex.wMax;
    model->tex.pos.x = WINDOW_WEIGHT - model->tex.weight - 10;
    model->tex.pos.y = (WINDOW_HEIGHT - model->tex.height) / 2;

    return 1;
}
