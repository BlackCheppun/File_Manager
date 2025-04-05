/**
 * @file TypeSuperBloc.h
 * @brief Définition de la structure du super-bloc
 * @author Ilyes AGHOUILES (50%) & Imane ABDELBASSIR (50%)
 * 
 * Ce fichier contient la définition du super-bloc qui maintient les informations
 * vitales sur la partition. Le super-bloc est la structure principale qui permet
 * de gérer l'organisation du système de fichiers, en gardant trace des ressources
 * disponibles et des limites du système.
 */
#ifndef TYPE_SUPERBLOC_H
#define TYPE_SUPERBLOC_H

/**
 * @brief Structure du super-bloc contenant les métadonnées de la partition
 * 
 * Le super-bloc est la structure principale qui contient toutes les informations
 * essentielles sur l'organisation et l'état de la partition. Il maintient les
 * compteurs de ressources (blocs, fichiers, dossiers) et leurs limites.
 */
typedef struct sb
{
    /// @brief Nombre total de blocs dans la partition
    unsigned short totalBlock;
    
    /// @brief Taille d'un bloc en octets
    unsigned short tailleBlock;
    
    /// @brief Nombre de blocs actuellement disponibles
    unsigned short nbBlockDispo;
    
    /// @brief Nombre maximum de fichiers supportés par la partition
    unsigned short totalFile;
    
    /// @brief Nombre de fichiers pouvant encore être créés
    unsigned short nbFileDispo;
    
    /// @brief Nombre maximum de dossiers supportés par la partition
    unsigned short totalDirectory;
    
    /// @brief Nombre de dossiers pouvant encore être créés
    unsigned short nbDirectoryDispo;
    
    /// @brief Identifiant du dossier racine
    unsigned short rootID;
    
    /// @brief Signature du système de fichiers (pour validation)
    /// @details Contient une chaîne de 4 caractères identifiant le type de système de fichiers
    char fs_name[4];
} SuperBlock;

#endif