#ifndef TYPE_FICHIER_H
#define TYPE_FICHIER_H
#include "util/constant.h"

typedef struct TypeFichier
{
    char nom[MAX_FILES_NAME_SIZE];
    unsigned int size;
    unsigned short posInBlockBMP;
} File;

#endif