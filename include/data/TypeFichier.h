/**
 * @file TypeFichier.h
 * @author Farah ALIANE
 * @author Laurent LIN
 * @brief Définition d'un fichier de notre partition
 */
#ifndef TYPE_FICHIER_H
#define TYPE_FICHIER_H
#include "util/constant.h"
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
} File;

#endif