#include "constants.h"
#include "model.h"
#include "render.h"

int initModel(Model *model)
{
    model->nbMembers = 0;
    model->member = NULL;
    model->translation = NULL;
    model->init = 1;

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
            fscanf(file, "Translation : %lf %lf %lf\n", &model->translation[i].x, &model->translation[i].y, &model->translation[i].z);
            for(j = 0; j < 6; j++)
            {
                fscanf(file, "face %d :\n", &j);
                fscanf(file, "c %lf %lf %lf\n\n", &model->member[i].face[j].color.r, &model->member[i].face[j].color.v, &model->member[i].face[j].color.b);
                for(k = 0; k < 4; k++)
                {
                    fscanf(file, "v %lf, %lf, %lf\n", &model->member[i].face[j].point[k].x, &model->member[i].face[j].point[k].y, &model->member[i].face[j].point[k].z);
                    fscanf(file, "t %lf %lf\n", &model->member[i].face[j].point[k].coordFileTexture.x, &model->member[i].face[j].point[k].coordFileTexture.y);
                }
            }
        }
    }

    return 1;
}

int saveModel(char *path, Model *model)
{
    FILE *file = fopen(path, "w+");
    int i, j, k;

    if(file == NULL)
    {
        printf("Saving model failed (%s)\n", path);
        return 0;
    }

    fprintf(file, "nbMembers = %d\n", model->nbMembers);

    for(i = 0; i < model->nbMembers; i++)
    {
        fprintf(file, "Member %d\n", i);
        fprintf(file, "Translation : %lf %lf %lf\n", model->translation[i].x, model->translation[i].y, model->translation[i].z);
        for(j = 0; j < 6; j++)
        {
            fprintf(file, "face %d :\n", j);
            fprintf(file, "c %lf %lf %lf\n\n", model->member[i].face[j].color.r, model->member[i].face[j].color.v, model->member[i].face[j].color.b);
            for(k = 0; k < 4; k++)
            {
                fprintf(file, "v %lf, %lf, %lf\n", model->member[i].face[j].point[k].x, model->member[i].face[j].point[k].y, model->member[i].face[j].point[k].z);
                fprintf(file, "t %lf %lf\n", model->member[i].face[j].point[k].coordFileTexture.x, model->member[i].face[j].point[k].coordFileTexture.y);
            }
        }
    }

    return 1;
}

int freeModel(Model *model)
{
    free(model->member);
    free(model->translation);

    return 1;
}

int renderModel(Model *model, int mode)
{
    int i, j;

    glPushMatrix();

    for(i = 0; i < model->nbMembers; i++)
    {
        glPushMatrix();
        glTranslated(model->translation[i].x, model->translation[i].y, model->translation[i].z);

        for(j = 0; j < 6; j++)
        {
            if(mode == RENDER_MODE)
                glColor3ub(model->member[i].face[j].color.r, model->member[i].face[j].color.v, model->member[i].face[j].color.b);
            else if(mode == COLLISION_MODE)
                glColor3ub(i + 10, j + 10, j + 10);//To find indexes in the collision with glReadPixels();

            drawFace(&model->member[i].face[j]);
        }
        glPopMatrix();
    }

    glPopMatrix();

    return 1;
}
