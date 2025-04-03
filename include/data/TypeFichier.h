/**
 * @file TypeFichier.h
 * @brief Définition d'un fichier de notre partition
 */
#ifndef TYPE_FICHIER_H
#define TYPE_FICHIER_H
#include "../util/constant.h"

// Link types
#define LINK_TYPE_NONE 0
#define LINK_TYPE_SYMBOLIC 1
#define LINK_TYPE_HARD 2

/**
 * @brief Un fichier est caractérisé par plusieurs informations :
 * Les 2 plus importantes sont : l'indice dans le tableau BlockBitmap
 * et l'indice virtuel dans le fichier.
 */
typedef struct TypeFichier
{
    /// @brief Nom du fichier
    char nom[MAX_FILES_NAME_SIZE];
    /// @brief Taille du fichier
    unsigned int size;
    /// @brief Indice dans le tableau Bitmap
    unsigned short posInBlockBMP;
    /// @brief Indice virtuel pour l'écriture et lecture
    unsigned int posSeek;
    /// @brief Indice du fichier dans le tableau de fichiers
    unsigned short index;
    /// @brief Indice du dossier parent
    unsigned short parentIndex;
    /// @brief Type de lien (0: pas de lien, 1: lien symbolique, 2: lien physique)
    unsigned char linkType;
    /// @brief Chemin cible pour les liens symboliques
    char targetPath[MAX_FILES_NAME_SIZE];
    /// @brief Index du fichier cible pour les liens physiques
    unsigned short targetFileIndex;
    /// @brief Permissions du fichier
    unsigned short permissions;
} File;

#endif