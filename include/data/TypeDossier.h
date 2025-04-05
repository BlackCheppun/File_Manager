/**
 * @file TypeDossier.h
 * @brief Définition des structures de données pour la gestion des dossiers
 * @author Ilyes AGHOUILES (100%)
 * Ce fichier contient les définitions des structures nécessaires pour
 * implémenter la hiérarchie des dossiers dans le système de fichiers.
 */
#ifndef TYPE_DOSSIER_H
#define TYPE_DOSSIER_H
#include "../data/TypeFichier.h"
#include "../util/constant.h"

/**
 * @brief Structure représentant une entrée dans un dossier
 * 
 * Cette structure permet de différencier si une entrée est un fichier
 * ou un sous-dossier, et stocke son index dans le bloc correspondant.
 */
typedef struct dirent
{
    /// @brief Flag indiquant si l'entrée est un dossier (1) ou un fichier (0)
    unsigned char isDirectory;
    /// @brief Index de l'entrée dans le bloc de fichiers ou de dossiers
    unsigned short index;
} DirectoryEntry;

/**
 * @brief Structure représentant un dossier dans le système de fichiers
 * 
 * Cette structure contient toutes les informations nécessaires pour gérer
 * un dossier, incluant son nom, son identifiant unique, son parent, et
 * les listes de ses fichiers et sous-dossiers.
 */
typedef struct TypeDossier
{
    /// @brief Nom du dossier
    char nomDossier[MAX_FILES_NAME_SIZE];
    /// @brief Identifiant unique du dossier
    unsigned short repoID;
    /// @brief Identifiant du dossier parent (0 pour la racine)
    unsigned short parentID;
    /// @brief Nombre de fichiers dans le dossier
    unsigned short nbFiles;
    /// @brief Nombre de sous-dossiers
    unsigned short nbSubRepos;
    /// @brief Tableau des identifiants des fichiers contenus dans le dossier
    unsigned short files[MAX_ENTRIES_DIR];
    /// @brief Tableau des identifiants des sous-dossiers
    unsigned short subRepos[MAX_ENTRIES_DIR];
} Directory;

#endif