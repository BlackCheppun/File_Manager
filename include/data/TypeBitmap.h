/**
 * @file TypeBitmap.h
 * @brief Définition de la structure de la bitmap des blocs
 * @author Massin KHELOUFI (30%) & Ilyes AGHOUILES ( ) & Imane ABDELBASSIR ( )
 * 
 * Ce fichier contient la définition de la structure BlockBitmap qui permet
 * de gérer l'allocation des blocs dans le système de fichiers. La bitmap
 * maintient les liens entre les blocs et permet de suivre les blocs libres
 * et occupés.
 */
#ifndef TYPE_BITMAP_H
#define TYPE_BITMAP_H
#include "../util/constant.h"

/**
 * @brief Structure représentant la bitmap des blocs
 * 
 * Cette structure maintient un tableau d'indices qui permet de suivre
 * l'enchaînement des blocs pour chaque fichier. Pour chaque bloc :
 * - Une valeur de 0 indique que le bloc est libre
 * - Une valeur de USHRT_MAX indique la fin d'un fichier
 * - Toute autre valeur représente l'index du prochain bloc du fichier
 */
typedef struct bmp
{
    /// @brief Tableau des indices de chaînage des blocs
    /// @details Chaque entrée contient soit :
    /// - 0 : bloc libre
    /// - USHRT_MAX : fin de fichier
    /// - autre valeur : index du bloc suivant
    unsigned short bmpTab[NUMBER_OF_BLOCK];
} BlockBitmap;

#endif