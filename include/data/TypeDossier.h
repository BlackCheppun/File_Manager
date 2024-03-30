/**
 * @file TypeDossier.h
 * @author Farah ALIANE
 * @author Laurent LIN
 * @brief Définition de l'implémentation des dossiers
 */
#ifndef TYPE_DOSSIER_H
#define TYPE_DOSSIER_H
#include "data/TypeFichier.h"
#include "util/constant.h"

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
}DirectoryEntry;
/**
 * @brief La structure possède quelques informations sur le dossier parent et ses enfants
 */
typedef struct TypeDossier
{
    /// @brief Nom du dossier
    char nomDossier[MAX_FILES_NAME_SIZE];
    /// @brief Nombre de dossier/fichier
    unsigned short nbEntry;
    /// @brief Indice du tableau qui contient le parent de ce dossier
    unsigned short parentIndex;
    /// @brief Stockage des enfants
    DirectoryEntry dirEnt[MAX_ENTRIES_DIR];
} Directory;

#endif