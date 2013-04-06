#include "constants.h"
#include "text.h"
#include "button.h"
#include "input.h"

extern Input event;
extern int windowWidth;
extern int windowHeight;
extern int weightLetter[256];

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

int renderButton(Button *button, Texture *textureText)
{
    Point2D position[4];
    Point2D posText;

    position[0].x = button->pos.x;
    position[0].y = windowHeight - button->pos.y;
    position[1].x = button->pos.x + button->weight;
    position[1].y = windowHeight - button->pos.y;
    position[2].x = button->pos.x + button->weight;
    position[2].y = windowHeight -(button->pos.y + button->height);
    position[3].x = button->pos.x;
    position[3].y = windowHeight -(button->pos.y + button->height);

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
        posText.x = (int)button->pos.x + button->weight / 2 - getWeightString((button->text), weightLetter) / 2;
        posText.y = (int)button->pos.y + button->height / 2 - 8;

        writeText(textureText, button->text, weightLetter, (int)posText.x, (int)posText.y);

        glPopMatrix();
    }

    return 1;
}

int buttonCollision(Button *button)
{
    button->selected = 0;

    if(event.posX >= (int)button->pos.x && event.posX <= (int)button->pos.x + button->weight)
    {
        if(event.posY >= (int)button->pos.y && event.posY <= (int)button->pos.y + button->height)
        {
            button->selected = 1;
            return 1;
        }
    }
    return 0;
}

void attribMainButtonsEditor(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_MAIN_BUTTONS_EDITOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "File");
    addStringToText(&button[1].text, "Edition");
    addStringToText(&button[2].text, "Tools");
    addStringToText(&button[3].text, "Texture");


    for(i = 0; i < NUMBER_MAIN_BUTTONS_EDITOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 10;
    button[0].pos.y = 10;

    button[1].pos.x = button[0].pos.x + button[0].weight + 10;
    button[1].pos.y = button[0].pos.y;

    button[2].pos.x = button[1].pos.x + button[1].weight + 10;
    button[2].pos.y = button[0].pos.y;

    button[3].pos.x = button[2].pos.x + button[2].weight + 10;
    button[3].pos.y = button[0].pos.y;
}

void attribEditionButtonsEditor(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_EDITION_BUTTONS_EDITOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Copy Cube (CTRL+C)");
    addStringToText(&button[1].text, "Paste Cube (CTRL+V)");
    addStringToText(&button[2].text, "Sculpt Mode");

    for(i = 0; i < NUMBER_EDITION_BUTTONS_EDITOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 63;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;

    button[2].pos.x = button[0].pos.x;
    button[2].pos.y = button[1].pos.y + button[1].height + 5;
}

void attribFileButtonsEditor(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_FILE_BUTTONS_EDITOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Open A Model");
    addStringToText(&button[1].text, "Save Model");
    addStringToText(&button[2].text, "Save As ...");
    addStringToText(&button[3].text, "Animate Mode");

    for(i = 0; i < NUMBER_FILE_BUTTONS_EDITOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 10;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;

    button[2].pos.x = button[0].pos.x;
    button[2].pos.y = button[1].pos.y + button[1].height + 5;

    button[3].pos.x = button[0].pos.x;
    button[3].pos.y = button[2].pos.y + button[2].height + 5;
}

void attribToolButtonsEditor(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_TOOL_BUTTONS_EDITOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Add A Cube");
    addStringToText(&button[1].text, "Remove Cube");
    addStringToText(&button[2].text, "Selection : Face");

    for(i = 0; i < NUMBER_TOOL_BUTTONS_EDITOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 148;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;

    button[2].pos.x = button[1].pos.x;
    button[2].pos.y = button[1].pos.y + button[1].height + 5;
}

void attribTextureButtonsEditor(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_TEXTURE_BUTTONS_EDITOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Apply A Texture");
    addStringToText(&button[1].text, "Invert Texture : X");
    addStringToText(&button[2].text, "Reverse Texture");

    for(i = 0; i < NUMBER_TEXTURE_BUTTONS_EDITOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 222;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;

    button[2].pos.x = button[0].pos.x;
    button[2].pos.y = button[1].pos.y + button[0].height + 5;
}

void attribMainButtonsAnimator(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_MAIN_BUTTONS_ANIMATOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "File");
    addStringToText(&button[1].text, "Edition");
    addStringToText(&button[2].text, "Animations");
    addStringToText(&button[3].text, "Selection");

    for(i = 0; i < NUMBER_MAIN_BUTTONS_ANIMATOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 10;
    button[0].pos.y = 10;

    button[1].pos.x = button[0].pos.x + button[0].weight + 10;
    button[1].pos.y = button[0].pos.y;

    button[2].pos.x = button[1].pos.x + button[1].weight + 10;
    button[2].pos.y = button[0].pos.y;

    button[3].pos.x = button[2].pos.x + button[2].weight + 10;
    button[3].pos.y = button[0].pos.y;
}

void attribFileButtonsAnimator(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_FILE_BUTTONS_ANIMATOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Open A Model");
    addStringToText(&button[1].text, "Save Model");
    addStringToText(&button[2].text, "Save As ...");
    addStringToText(&button[3].text, "Creation Mode");

    for(i = 0; i < NUMBER_FILE_BUTTONS_ANIMATOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 10;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;

    button[2].pos.x = button[0].pos.x;
    button[2].pos.y = button[1].pos.y + button[1].height + 5;

    button[3].pos.x = button[0].pos.x;
    button[3].pos.y = button[2].pos.y + button[2].height + 5;
}

void attribEditionButtonsAnimator(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_EDITION_BUTTONS_ANIMATOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Add Animation");
    addStringToText(&button[1].text, "Remove Animation");
    addStringToText(&button[2].text, "Animation : Rotation");
    addStringToText(&button[3].text, "Define Origin");

    for(i = 0; i < NUMBER_EDITION_BUTTONS_ANIMATOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 64;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;

    button[2].pos.x = button[1].pos.x;
    button[2].pos.y = button[1].pos.y + button[1].height + 5;

    button[3].pos.x = button[2].pos.x;
    button[3].pos.y = button[2].pos.y + button[2].height + 5;
}

void attribAnimationButtonsAnimator(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_ANIMATION_BUTTONS_ANIMATOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "No Animation Yet");

    for(i = 0; i < NUMBER_ANIMATION_BUTTONS_ANIMATOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 148;
    button[0].pos.y = 55;
}

void attribToolButtonsAnimator(Button *button, Texture *texButton)
{
    int i;

    for(i = 0; i < NUMBER_TOOL_BUTTONS_ANIMATOR; i++)
    {
        initButton(&button[i], texButton);
    }

    addStringToText(&button[0].text, "Select A Cube");
    addStringToText(&button[1].text, "Stop Selection");

    for(i = 0; i < NUMBER_TOOL_BUTTONS_ANIMATOR; i++)
    {
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
    }

    button[0].pos.x = 268;
    button[0].pos.y = 55;

    button[1].pos.x = button[0].pos.x;
    button[1].pos.y = button[0].pos.y + button[0].height + 5;
}

int attribAnimationButtonsAnimatorWithModelsAnimation(Model *model, Button *button, Texture *texButton)
{
    int i;

    if(model->nbAnims == 0)
    {
        printf("No anims on the model\n");
        return 0;
    }

    if(button == NULL)
    {
        printf("Error allocating button's memory\n");
        return 0;
    }

    initButton(&button[0], texButton);
    addStringToText(&button[0].text, "Stop animation");
    button[0].weight = getWeightString(button[0].text, weightLetter) + 10;
    button[0].height = 40;
    button[0].pos.x = 148;
    button[0].pos.y = 55;

    for(i = 1; i < model->nbAnims + 1; i++)
    {
        initButton(&button[i], texButton);
        addStringToText(&button[i].text, model->animation[i - 1]->animationName);
        button[i].weight = getWeightString(button[i].text, weightLetter) + 10;
        button[i].height = 40;
        button[i].pos.x = 148;
        button[i].pos.y = 55 + i * 45;
    }

    return 1;
}
