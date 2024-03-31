/**
 * @file TypeSuperBloc.h
 * @author Farah ALIANE
 * @author Laurent LIN
 * @brief Le SuperBloc contient les informations vitales sur la partition mais 
 * aucune informations sur le contenu de la partition 
 */
#ifndef TYPE_SUPERBLOC_H
#define TYPE_SUPERBLOC_H
/**
 * @brief Contient l'ensemble des informations de la partition
 */
typedef struct sb{
    /// @brief Nombre total de bloc
    unsigned short totalBlock;
    /// @brief Taille d'un bloc
    unsigned short tailleBlock;
    /// @brief Nombre de bloc disponible par rapport au nombre total
    unsigned short nbBlockDispo;
    /// @brief Nombre total de fichier possible par rapport au nombre de bloc
    unsigned short totalFile;
    /// @brief Nombre restant de fichier créable
    unsigned short nbFileDispo;
    /// @brief Nombre total de dossier possible par rapport au bloc Dossier
    unsigned short totalDirectory;
    /// @brief Nombre restant de dossier créable
    unsigned short nbDirectoryDispo;
    /// @brief Marqueur de propriété (pour éviter de casser d'autre fichier)
    char fs_name[4];
} SuperBlock;

#endif