#ifndef FICHIER_H
#define FICHIER_H

#include "data/TypeFichier.h"

/// @brief formatage de la partition 
/// @param nomPartition 
/// @return 0/-1
int myFormat(char* nomPartition);
int myClose(int file);

#endif