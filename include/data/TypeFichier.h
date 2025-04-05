/**
 * @file TypeFichier.h
 * @brief Définition de la structure des fichiers
 * @author Massin KHELOUFI (40%) & Ilyes AGHOUILES ( ) & Imane ABDELBASSIR ( )
 * 
 * Ce fichier contient la définition de la structure File qui représente
 * un fichier dans le système de fichiers. Il gère également les différents
 * types de liens (symboliques et physiques) et les permissions des fichiers.
 */
#ifndef TYPE_FICHIER_H
#define TYPE_FICHIER_H
#include "../util/constant.h"

/// @brief Pas de lien
#define LINK_TYPE_NONE 0
/// @brief Lien symbolique
#define LINK_TYPE_SYMBOLIC 1
/// @brief Lien physique
#define LINK_TYPE_HARD 2

/**
 * @brief Structure représentant un fichier dans le système de fichiers
 * 
 * Cette structure contient toutes les informations nécessaires pour gérer
 * un fichier, incluant ses métadonnées (nom, taille, permissions), sa position
 * dans la partition, et les informations sur les liens éventuels.
 */
typedef struct TypeFichier
{
    /// @brief Nom du fichier
    char nom[MAX_FILES_NAME_SIZE];
    
    /// @brief Taille du fichier en octets
    unsigned int size;
    
    /// @brief Index dans la table des blocs (bitmap)
    /// @details Pointe vers le premier bloc de données du fichier
    unsigned short posInBlockBMP;
    
    /// @brief Position courante pour la lecture/écriture
    /// @details Utilisé pour maintenir la position du curseur dans le fichier
    unsigned int posSeek;
    
    /// @brief Index du fichier dans la table des fichiers
    unsigned short index;
    
    /// @brief Index du dossier parent
    unsigned short parentIndex;
    
    /// @brief Type de lien
    /// @details Peut être :
    /// - LINK_TYPE_NONE (0) : fichier normal
    /// - LINK_TYPE_SYMBOLIC (1) : lien symbolique
    /// - LINK_TYPE_HARD (2) : lien physique
    unsigned char linkType;
    
    /// @brief Chemin cible pour les liens symboliques
    /// @details Contient le chemin absolu ou relatif vers le fichier cible
    char targetPath[MAX_FILES_NAME_SIZE];
    
    /// @brief Index du fichier cible pour les liens physiques
    /// @details Utilisé uniquement si linkType == LINK_TYPE_HARD
    unsigned short targetFileIndex;
    
    /// @brief Permissions du fichier
    /// @details Format Unix-like (rwx) codé sur 9 bits
    unsigned short permissions;
} File;

#endif