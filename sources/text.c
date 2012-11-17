#include "constants.h"
#include "text.h"
#include "render.h"

void loadWeightLetters(char *path, int *weightLetter)
{
    FILE *file = NULL;
    int n, i;

    file = fopen(path, "r");

    for(i = 0; i < 256; i++)
    {
        weightLetter[i] = 16;
    }

    if(file != NULL)
    {
        for(i = 0; n != EOF && i < 256; i++)
        {
            fscanf(file, "width.%d=", &n);
            fscanf(file, "%d\n", &weightLetter[n]);
        }
    }
    else
        printf("Cannot open file : %s\n", path);

    for(i = 0; i < 256; i++)
    {
        if(weightLetter[i] == 2)
            weightLetter[i] = 2;
        else if(weightLetter[i] == 3)
            weightLetter[i] = 4;
        else if(weightLetter[i] == 4)
            weightLetter[i] = 6;
        else if(weightLetter[i] == 5)
            weightLetter[i] = 8;
        else if(weightLetter[i] == 6)
            weightLetter[i] = 10;
        else if(weightLetter[i] == 7)
            weightLetter[i] = 12;
        else if(weightLetter[i] == 8)
            weightLetter[i] = 8;
    }

    weightLetter[32] = 5;

    fclose(file);
}

void writeText(Texture *texture, Text text, int *weightLetter, int x, int y)
{
    int posX, posY;
    int i;

    texture->weight = 0;
    texture->height = 16;

    glPushMatrix();

    if(text.shadow == 1)
    {
        texture->pos.x = x;
        texture->pos.y = y + 2;

        texture->color.r = 50;
        texture->color.v = 50;
        texture->color.b = 50;

        for(i = 0; i < text.nbChar; i++)
        {
            posY = text.string[i] / 16;
            posX = text.string[i] - posY * 16;

            texture->pos.x += texture->weight + 2;

            texture->weight = weightLetter[(int)text.string[i]];

            texture->posTex[0].x = (double)((posX * 16)) / 256;
            texture->posTex[0].y = (double)(256 - (posY) * 16) / 256;
            texture->posTex[1].x = (double)(posX * 16 + texture->weight) / 256;
            texture->posTex[1].y = (double)(256 - (posY + 1) * 16) / 256;


            texture->pos.x += 2;

            if(text.string[i] != ' ' && text.string[i] != 0)
            {
                drawTexture(texture);
            }

            texture->pos.x -= 2;
        }
    }

    glTranslated(0, 0, 1);
    texture->color.r = 255;
    texture->color.v = 255;
    texture->color.b = 255;

    texture->pos.x = x;
    texture->pos.y = y;
    texture->weight = 0;

    for(i = 0; i < text.nbChar; i++)
    {
        posY = text.string[i] / 16;
        posX = text.string[i] - posY * 16;

        texture->pos.x += texture->weight + 2;

        texture->weight = weightLetter[(int)text.string[i]];

        texture->posTex[0].x = (double)((posX * 16)) / 256;
        texture->posTex[0].y = (double)(256 - (posY) * 16) / 256;
        texture->posTex[1].x = (double)(posX * 16 + texture->weight) / 256;
        texture->posTex[1].y = (double)(256 - (posY + 1) * 16) / 256;

        if(text.string[i] != ' ' && text.string[i] != 0)
        {
            drawTexture(texture);
        }
    }

    glPopMatrix();
}

int getWeightString(Text text, int *weightLetter)
{
    int i;
    int weight = 0;

    for(i = 0; i < text.nbChar; i++)
    {
        if(text.string[i] != 0)
            weight += weightLetter[(int)text.string[i]] + 2;
    }

    return weight;
}

int addStringToText(Text *text, char *string)
{
    if(text->string != NULL)
    {
        free(text->string);
    }

    text->nbChar = strlen(string);
    text->string = malloc((text->nbChar + 1) * sizeof(char));

    if(text->string == NULL)
    {
        printf("Impossible to allocate memory for string");
        return 0;
    }

    sprintf(text->string, "%s", string);
    return 1;
}

int addCharToString(Text *text, int cursor)
{
    char *tmp;

    tmp = malloc(text->nbChar * sizeof(char));

    return 1;
}

int getText()
{
    /*int cursor = 0;
    int tpsActuel = 0, tpsPrecedent = 0;
    char c = 0;

    while(event.touche[SDLK_RETURN] == 0 && event.touche[SDLK_KP_ENTER] == 0)
    {
        updateEvents(&event);

        tpsActuel = SDL_GetTicks();

        if(tpsActuel > tpsPrecedent + 20)
        {
            c = interpreterEvent();

            if(c != 0)
            {
                texte[curseur] = c;
                curseur++;
                c = 0;
            }

            if(event.touche[SDLK_BACKSPACE] == 1)
            {
                if(curseur > 0)
                {
                    curseur--;
                    texte[curseur] = 0;
                }
                event.touche[SDLK_BACKSPACE] = 0;
            }
            tpsPrecedent = tpsActuel;
        }
        printf("Actuel (%d) : %s_\n", tpsActuel, texte);
    }*/
    return 1;
}
