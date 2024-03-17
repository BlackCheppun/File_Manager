#ifndef TYPE_DOSSIER_H
#define TYPE_DOSSIER_H
#include "data/TypeFichier.h"
#include "util/constant.h"

typedef struct dirent
{
    unsigned char isDirectory;
    unsigned short index;
}DirectoryEntry;

typedef struct TypeDossier
{
    char nomDossier[MAX_FILES_NAME_SIZE];
    unsigned short nbEntry;
    unsigned short parentIndex;
    DirectoryEntry dirEnt[MAX_ENTRIES_DIR];
} Directory;

#endif