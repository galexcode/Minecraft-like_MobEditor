#include "constants.h"
#include "button.h"
#include "text.h"
#include "input.h"

extern Input event;

int initButton(Button *button, Texture *tex)
{
    int i;

    button->IDtex = tex->IDtex;
    button->pos.x = 0;
    button->pos.y = 0;
    button->wMax = tex->weight;
    button->hMax = tex->height;
    button->weight = 0;
    button->height = 0;
    button->selected = 0;
    button->textInput = 0;
    button->text.nbChar = 0;
    button->text.string[0] = 0;
    button->text.shadow = 1;
    button->textureUnselected[0].x = 0;
    button->textureUnselected[0].y = 132;
    button->textureUnselected[1].x = 399;
    button->textureUnselected[1].y = 171;
    button->textureSelected[0].x = 0;
    button->textureSelected[0].y = 172;
    button->textureSelected[1].x = 399;
    button->textureSelected[1].y = 211;//SDL coords

    for(i = 0; i < 2; i++)
    {
        button->textureUnselected[i].x = button->textureUnselected[i].x / 512;
        button->textureUnselected[i].y = (512 - button->textureUnselected[i].y) / 512;
        button->textureSelected[i].x = button->textureSelected[i].x / 512;
        button->textureSelected[i].y = (512 - button->textureSelected[i].y) / 512;
    }

    return 1;
}

int renderButton(Button *button, Texture *textureText, int *weightLetter, Text *buff)
{
    Point2D position[4];
    Point2D posText;

    position[0].x = button->pos.x;
    position[0].y = WINDOW_HEIGHT - button->pos.y;
    position[1].x = button->pos.x + button->weight;
    position[1].y = WINDOW_HEIGHT - button->pos.y;
    position[2].x = button->pos.x + button->weight;
    position[2].y = WINDOW_HEIGHT -(button->pos.y + button->height);
    position[3].x = button->pos.x;
    position[3].y = WINDOW_HEIGHT -(button->pos.y + button->height);

    if(button->textInput == 0)
    {
        glColor3ub(255, 255, 255);

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, button->IDtex);

        if(button->selected == 1)
        {
            glBegin(GL_QUADS);

            glTexCoord2d(button->textureSelected[0].x, button->textureSelected[0].y);
            glVertex2i((int)position[0].x, (int)position[0].y);
            glTexCoord2d(button->textureSelected[1].x, button->textureSelected[0].y);
            glVertex2i((int)position[1].x, (int)position[1].y);
            glTexCoord2d(button->textureSelected[1].x, button->textureSelected[1].y);
            glVertex2i((int)position[2].x, (int)position[2].y);
            glTexCoord2d(button->textureSelected[0].x, button->textureSelected[1].y);
            glVertex2i((int)position[3].x, (int)position[3].y);

            glEnd();
        }
        else
        {
            glBegin(GL_QUADS);

            glTexCoord2d(button->textureUnselected[0].x, button->textureUnselected[0].y);
            glVertex2i((int)position[0].x, (int)position[0].y);
            glTexCoord2d(button->textureUnselected[1].x, button->textureUnselected[0].y);
            glVertex2i((int)position[1].x, (int)position[1].y);
            glTexCoord2d(button->textureUnselected[1].x, button->textureUnselected[1].y);
            glVertex2i((int)position[2].x, (int)position[2].y);
            glTexCoord2d(button->textureUnselected[0].x, button->textureUnselected[1].y);
            glVertex2i((int)position[3].x, (int)position[3].y);

            glEnd();
        }

        if(button->text.string != NULL)
        {
            glTranslated(0, 0, 1);
            posText.x = (int)button->pos.x + button->weight / 2 - getWeightString(button->text, weightLetter) / 2;
            posText.y = (int)button->pos.y + button->height / 2 - 8;

            glPushMatrix();
            writeText(textureText, button->text, weightLetter, (int)posText.x, (int)posText.y);
            glPopMatrix();
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glDisable(GL_TEXTURE_2D);

    if(button->textInput == 1)
    {
        glPushMatrix();

        glBegin(GL_QUADS);

        glColor3ub(10, 10, 10);
        glVertex2i((int)position[0].x, (int)position[0].y);
        glVertex2i((int)position[1].x, (int)position[1].y);
        glVertex2i((int)position[2].x, (int)position[2].y);
        glVertex2i((int)position[3].x, (int)position[3].y);

        glTranslated(0, 0, 1);

        glColor3ub(255, 255, 255);
        glVertex2i((int)position[0].x - 2, (int)position[0].y + 2);
        glVertex2i((int)position[1].x + 2, (int)position[1].y + 2);
        glVertex2i((int)position[2].x + 2, (int)position[2].y - 2);
        glVertex2i((int)position[3].x - 2, (int)position[3].y - 2);

        glEnd();

        glTranslated(0, 0, 1);
        posText.x = (int)button->pos.x + button->weight / 2 - getWeightString((*buff), weightLetter) / 2;
        posText.y = (int)button->pos.y + button->height / 2 - 8;

        writeText(textureText, (*buff), weightLetter, (int)posText.x, (int)posText.y);

        glPopMatrix();
    }

    return 1;
}

void buttonCollision(Button *button)
{
    button->selected = 0;

    if(event.posX >= (int)button->pos.x && event.posX <= (int)button->pos.x + button->weight)
    {
        if(event.posY >= (int)button->pos.y && event.posY <= (int)button->pos.y + button->height)
        {
            button->selected = 1;
        }
    }
}

void attribButtons(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_BUTTONS_EDITOR; i++)
    {
        initButton(&button[i], texButton);
    }

    button[0].height = 40;
    button[0].weight = 130;
    button[0].pos.x = 10;
    button[0].pos.y = 10;
    addStringToText(&button[0].text, "Add a cube");

    button[1].height = 40;
    button[1].weight = 180;
    button[1].pos.x = 10;
    button[1].pos.y = 55;
    addStringToText(&button[1].text, "Selection : Face");

    button[2].height = 40;
    button[2].weight = 150;
    button[2].pos.x = 10;
    button[2].pos.y = WINDOW_HEIGHT - button[2].height - 10;
    addStringToText(&button[2].text, "Save Model");

    button[3].height = 40;
    button[3].weight = 170;
    button[3].pos.x = 170;
    button[3].pos.y = WINDOW_HEIGHT - button[3].height - 10;
    addStringToText(&button[3].text, "Open new model");

    button[4].height = 40;
    button[4].weight = 170;
    button[4].pos.x = WINDOW_WEIGHT - button[4].weight - 10;
    button[4].pos.y = 10;
    addStringToText(&button[4].text, "Apply a texture");

    button[5].height = 40;
    button[5].weight = 200;
    button[5].pos.x = WINDOW_WEIGHT - button[5].weight - 10;
    button[5].pos.y = WINDOW_HEIGHT - button[5].height - 10;
    addStringToText(&button[5].text, "Invert Texture : X");
}
