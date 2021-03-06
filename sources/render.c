#include "constants.h"
#include "render.h"

extern int windowWidth;
extern int windowHeight;

void lookAt(Point3D *position, Point3D *target)
{
    gluLookAt(position->x, position->y, position->z, target->x, target->y, target->z, 0, 1, 0);
}

void drawCube(Cube *cube)
{
    int i;

    for(i = 0; i < 6; i++)
    {
        drawFace(&cube->face[i], RENDER_MODE);
    }
}

void drawFace(Face *face, int mode)
{
    int i;

    if(face->point[0].coordFileTexture.x != (double)-1 && mode == RENDER_MODE)
    {
        glEnable(GL_TEXTURE_2D);
    }

    glBegin(GL_QUADS);
    for(i = 0; i < 4; i++)
    {
        if(face->point[i].coordFileTexture.x != (double)-1 && mode == RENDER_MODE)
        {
            glTexCoord2f(face->point[i].coordFileTexture.x, face->point[i].coordFileTexture.y);
        }
        glVertex3f(face->point[i].x, face->point[i].y, face->point[i].z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void clearScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void modeRender(int modeScene, Point3D *pos, Point3D *target, int FOV)
{
    if(modeScene == RENDER_3D)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(FOV, (double)windowWidth/windowHeight, 0.01, 1000);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        lookAt(pos, target);
    }
    else if(modeScene == RENDER_2D)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(0, windowWidth, 0, windowHeight, 0, 10000);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

void refreshScene()
{
    glFlush();
    SDL_GL_SwapBuffers();
}

void drawTexture(Texture *tex)
{
    Point2D position[4];

    position[0].x = tex->pos.x;
    position[0].y = windowHeight - tex->pos.y;
    position[1].x = tex->pos.x + tex->weight;
    position[1].y = windowHeight - tex->pos.y;
    position[2].x = tex->pos.x + tex->weight;
    position[2].y = windowHeight - (tex->pos.y + tex->height);
    position[3].x = tex->pos.x;
    position[3].y = windowHeight -(tex->pos.y + tex->height);

    glColor3ub(tex->color.r, tex->color.v, tex->color.b);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, tex->IDtex);

    glBegin(GL_QUADS);

    glTexCoord2d(tex->posTex[0].x, tex->posTex[0].y);
    glVertex2d(position[0].x, position[0].y);
    glTexCoord2d(tex->posTex[1].x, tex->posTex[0].y);
    glVertex2d(position[1].x, position[1].y);
    glTexCoord2d(tex->posTex[1].x, tex->posTex[1].y);
    glVertex2d(position[2].x, position[2].y);
    glTexCoord2d(tex->posTex[0].x, tex->posTex[1].y);
    glVertex2d(position[3].x, position[3].y);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
