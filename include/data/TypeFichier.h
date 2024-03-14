#ifndef TYPE_FICHIER_H
#define TYPE_FICHIER_H

typedef struct TypeFichier
{
    char* nom;
    int size;
    int position;
    int isDirectory;
} File;


#endif