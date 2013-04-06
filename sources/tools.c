#include "constants.h"
#include "tools.h"

int openBrowser(char *path, int typeFile)
{
    #if defined WIN32
        int res;

        OPENFILENAME ofn;
        TCHAR tmp[SIZE_PATH_MAX] ;
        tmp[0]= '\0' ;
        ZeroMemory(&ofn , sizeof (OPENFILENAMEW));
        ofn.lStructSize = sizeof (OPENFILENAMEW);
        ofn.lpstrFile = tmp;
        ofn.nMaxFile = SIZE_PATH_MAX;
        ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;

        if(typeFile == MODELS)
            ofn.lpstrFilter = _T("Minecraft-like Models (*.mclmdl)\0");
        else if(typeFile == TEXTURES)
            ofn.lpstrFilter = _T("IMG File\0");

        res = GetOpenFileName(&ofn);

        if(res == 1)
        {
            sprintf(path, "%s", ofn.lpstrFile);
        }
        else
        {
            path[0] = '\0';
            return 0;
        }
    #endif

    return 1;
}

int getNameFileFromPath(char *mainPath, char *path, char *name)
{
    int i, j = 0;
    char c = 1;

    for(i = 0; i < SIZE_PATH_MAX && c != 0; i++)
    {
        c = path[i];
        if(c != mainPath[i])
        {
            name[j] = c;
            j++;
        }
    }

    name[j] = 0;

    return 1;
}

double getMinimal(double A, double B)
{
    if(A >= B)
        return B;
    else
        return A;
}

double getMaximal(double A, double B)
{
    if(A >= B)
        return A;
    else
        return B;
}
