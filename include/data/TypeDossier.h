/**
 * @file TypeDossier.h
 * @brief Définition de l'implémentation des dossiers
 */
#ifndef TYPE_DOSSIER_H
#define TYPE_DOSSIER_H
#include "../data/TypeFichier.h"
#include "../util/constant.h"

/**
 * @brief Un sous-élément (enfant) du dossier contient 2 informations
 * pour savoir si il faut chercher dans le FileBlock ou DirBlock
 *
 */
typedef struct dirent
{
    /// @brief Indique si c'est un dossier
    unsigned char isDirectory;
    /// @brief Indice du fichier/dossier dans le Bloc
    unsigned short index;
} DirectoryEntry;
/**
 * @brief La structure possède quelques informations sur le dossier parent et ses enfants
 */
typedef struct TypeDossier
{
    char nomDossier[MAX_FILES_NAME_SIZE];     // Directory name
    unsigned short repoID;                    // Unique directory ID
    unsigned short parentID;                  // Parent directory ID (0 = root)
    unsigned short nbFiles;                   // Number of files
    unsigned short nbSubRepos;                // Number of subdirectories
    unsigned short files[MAX_ENTRIES_DIR];    // File IDs in this directory
    unsigned short subRepos[MAX_ENTRIES_DIR]; // Subdirectory IDs
} Directory;

#endif