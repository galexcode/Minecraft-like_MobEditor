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
            fscanf(file, "Translation : %f %f %f\n", &model->translation[i]->x, &model->translation[i]->y, &model->translation[i]->z);
            fscanf(file, "Rotation : %f %f %f\n", &model->rotation[i]->x, &model->rotation[i]->y, &model->rotation[i]->z);
            for(j = 0; j < 6; j++)
            {
                fscanf(file, "face %d :\n", &j);
                fscanf(file, "c %lf %lf %lf\n\n", &model->member[i]->face[j].color.r, &model->member[i]->face[j].color.v, &model->member[i]->face[j].color.b);
                for(k = 0; k < 4; k++)
                {
                    fscanf(file, "v %f, %f, %f\n", &model->member[i]->face[j].point[k].x, &model->member[i]->face[j].point[k].y, &model->member[i]->face[j].point[k].z);
                    fscanf(file, "t %lf %lf\n\n", &model->member[i]->face[j].point[k].coordFileTexture.x, &model->member[i]->face[j].point[k].coordFileTexture.y);
                }
            }
        }
    }

    fscanf(file, "nbAnims = %d\n\n", &model->nbAnims);

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
        fscanf(file, "nbMembersAffected : %d\n\n", &model->animation[i]->nbMembersAffected);

        for(j = 0; j < model->animation[i]->nbMembersAffected; j++)
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
        }

        model->animation[i]->lastUpdate = -1;
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
        fprintf(file, "Translation : %f %f %f\n", model->translation[i]->x, model->translation[i]->y, model->translation[i]->z);
        fprintf(file, "Rotation : %f %f %f\n", model->rotation[i]->x, model->rotation[i]->y, model->rotation[i]->z);
        for(j = 0; j < 6; j++)
        {
            fprintf(file, "face %d :\n", j);
            fprintf(file, "c %lf %lf %lf\n\n", model->member[i]->face[j].color.r, model->member[i]->face[j].color.v, model->member[i]->face[j].color.b);
            for(k = 0; k < 4; k++)
            {
                fprintf(file, "v %f, %f, %f\n", model->member[i]->face[j].point[k].x, model->member[i]->face[j].point[k].y, model->member[i]->face[j].point[k].z);
                fprintf(file, "t %lf %lf\n\n", model->member[i]->face[j].point[k].coordFileTexture.x, model->member[i]->face[j].point[k].coordFileTexture.y);
            }
        }
    }

    fprintf(file, "nbAnims = %d\n\n", model->nbAnims);

    for(i = 0; i < model->nbAnims; i++)
    {
        fprintf(file, "anim %d\n", i);
        fprintf(file, "name : %s\n", model->animation[i]->animationName);
        fprintf(file, "nbMembersAffected : %d\n\n", model->animation[i]->nbMembersAffected);

        for(j = 0; j < model->animation[i]->nbMembersAffected; j++)
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
    int i, j;

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, model->tex.IDtex);

    for(i = 0; i < model->nbMembers; i++)
    {
        glPushMatrix();
        glTranslatef(model->translation[i]->x, model->translation[i]->y, model->translation[i]->z);
        glRotated(model->rotation[i]->x, 1, 0, 0);
        glRotated(model->rotation[i]->y, 0, 1, 0);
        glRotated(model->rotation[i]->z, 0, 0, 1);

        for(j = 0; j < 6; j++)
        {
            if(mode == RENDER_MODE)
            {
                glEnable(GL_BLEND);
                glColor3ub(model->member[i]->face[j].color.r, model->member[i]->face[j].color.v, model->member[i]->face[j].color.b);
            }
            else if(mode == COLLISION_MODE)
            {
                glColor3ub(i + 10, j + 10, j + 10);//To find indexes in the collision with glReadPixels();
            }

            drawFace(&model->member[i]->face[j], mode);
            glDisable(GL_BLEND);
        }
        glPopMatrix();
    }

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

    return 1;
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
    float *dataToAnimate = NULL;

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

    for(i = 0; i < model->animation[indexAnimation]->nbMembersAffected; i++)
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

        if(model->animation[indexAnimation]->currentPhase[i] == INCREASING)
        {
            (*dataToAnimate) += ((SDL_GetTicks() - model->animation[indexAnimation]->lastUpdate) * (model->animation[indexAnimation]->maximalValue[i] - model->animation[indexAnimation]->minimalValue[i])) / model->animation[indexAnimation]->period[i];
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
            (*dataToAnimate) -= ((SDL_GetTicks() - model->animation[indexAnimation]->lastUpdate) * (model->animation[indexAnimation]->maximalValue[i] - model->animation[indexAnimation]->minimalValue[i])) / model->animation[indexAnimation]->period[i];
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

        for(i = 0; i < model->animation[indexAnimation]->nbMembersAffected; i++)
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
