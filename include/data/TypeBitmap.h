/**
 * @file TypeBitmap.h
 * @author Farah ALIANE
 * @author Laurent LIN
 * @brief Définition de la structure blockbitmap
 */
#ifndef TYPE_BITMAP_H
#define TYPE_BITMAP_H
#include "../util/constant.h"
/**
 * @brief Cette structure contient l'indice du
 * prochain bloc où trouver l'information, si elle
 * contient USHRT_MAX alors on arrive à la fin du fichier.<br/>
 * Contient 0 si le bloc est libre.
 */
typedef struct bmp
{
    /**
     * @brief Tableau contenant les indices
     */
    unsigned short bmpTab[NUMBER_OF_BLOCK];
} BlockBitmap;

#endif