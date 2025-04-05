/**
 * @file constant.h
 * @brief Définition des constantes du système de fichiers
 * @author Massin KHELOUFI (20%) & Ilyes AGHOUILES (20%) & Imane ABDELBASSIR (60%)
 * 
 * Ce fichier contient toutes les constantes qui définissent les paramètres
 * du système de fichiers, incluant les tailles des blocs, les offsets des
 * différentes sections, et les limites du système.
 * Ces constantes peuvent être redéfinies avant l'inclusion du fichier.
 */

/**
 * @brief Nom du fichier représentant la partition virtuelle
 * @details Variable externe définie ailleurs dans le programme
 */
extern char* PARTITION_NAME;

#ifndef BLOCK_SIZE
/// @brief Taille d'un bloc en octets
/// @details Unité de base pour l'allocation de l'espace disque
#define BLOCK_SIZE 4096 
#endif

#ifndef NUMBER_OF_BLOCK
/// @brief Nombre total de blocs de données dans la partition
#define NUMBER_OF_BLOCK 2048
#endif

#ifndef SUPERBLOCK_SIZE
/// @brief Taille du super-bloc en octets
#define SUPERBLOCK_SIZE 64 
#endif

#ifndef BLOCK_BITMAP_ARRAY_SIZE
/// @brief Nombre d'entrées dans la bitmap des blocs
/// @details Doit correspondre au nombre total de blocs
#define BLOCK_BITMAP_ARRAY_SIZE NUMBER_OF_BLOCK
#endif

#ifndef BLOCK_BITMAP_SIZE
/// @brief Taille totale de la bitmap en octets
#define BLOCK_BITMAP_SIZE NUMBER_OF_BLOCK*sizeof(unsigned short)
#endif

#ifndef FILE_BLOCK_SIZE
/// @brief Taille de la zone de stockage des métadonnées des fichiers
/// @details Calculée pour pouvoir stocker les métadonnées de tous les fichiers possibles
#define FILE_BLOCK_SIZE NUMBER_OF_BLOCK * sizeof(File)
#endif

#ifndef MAX_DIR_AMOUNT
/// @brief Nombre maximum de dossiers dans la partition
#define MAX_DIR_AMOUNT 256
#endif

#ifndef DIR_BLOCK_SIZE
/// @brief Taille de la zone de stockage des métadonnées des dossiers
/// @details Calculée pour pouvoir stocker tous les dossiers possibles
#define DIR_BLOCK_SIZE MAX_DIR_AMOUNT * sizeof(Directory)
#endif

#ifndef PARTITION_SIZE
/// @brief Taille totale de la partition en octets
/// @details Somme des tailles de toutes les zones (super-bloc, bitmap, métadonnées, données)
/// @note La taille maximale de la bitmap est limitée à 65536 (taille max d'un unsigned short)
#define PARTITION_SIZE (SUPERBLOCK_SIZE + BLOCK_BITMAP_SIZE + FILE_BLOCK_SIZE + DIR_BLOCK_SIZE + BLOCK_SIZE * NUMBER_OF_BLOCK)
#endif

#ifndef SUPERBLOCK_OFFSET
/// @brief Offset du super-bloc depuis le début de la partition
#define SUPERBLOCK_OFFSET 0
#endif

#ifndef BITMAPBLOCK_OFFSET
/// @brief Offset de la bitmap depuis le début de la partition
#define BITMAPBLOCK_OFFSET SUPERBLOCK_SIZE
#endif

#ifndef FILEBLOCK_OFFSET
/// @brief Offset de la zone des métadonnées des fichiers
#define FILEBLOCK_OFFSET (BITMAPBLOCK_OFFSET + BLOCK_BITMAP_SIZE)
#endif 

#ifndef DIRBLOCK_OFFSET
/// @brief Offset de la zone des métadonnées des dossiers
#define DIRBLOCK_OFFSET (FILEBLOCK_OFFSET + FILE_BLOCK_SIZE)
#endif 

#ifndef DATABLOCK_OFFSET 
/// @brief Offset du début de la zone de données
#define DATABLOCK_OFFSET (DIRBLOCK_OFFSET + DIR_BLOCK_SIZE)
#endif 

#ifndef MAX_ENTRIES_DIR 
/// @brief Nombre maximum d'entrées dans un dossier
/// @details Limite le nombre de fichiers et sous-dossiers par dossier
#define MAX_ENTRIES_DIR 64
#endif 

#ifndef MAX_FILES_NAME_SIZE 
/// @brief Taille maximum du nom d'un fichier en caractères
#define MAX_FILES_NAME_SIZE 64
#endif 

/**
 * @name Constantes de permissions
 * @brief Définition des masques de bits pour les permissions Unix
 * @{
 */
/// @brief Permission de lecture pour le propriétaire
#define PERM_OWNER_READ   0400
/// @brief Permission d'écriture pour le propriétaire
#define PERM_OWNER_WRITE  0200
/// @brief Permission d'exécution pour le propriétaire
#define PERM_OWNER_EXEC   0100
/// @brief Permission de lecture pour le groupe
#define PERM_GROUP_READ   0040
/// @brief Permission d'écriture pour le groupe
#define PERM_GROUP_WRITE  0020
/// @brief Permission d'exécution pour le groupe
#define PERM_GROUP_EXEC   0010
/// @brief Permission de lecture pour les autres
#define PERM_OTHER_READ   0004
/// @brief Permission d'écriture pour les autres
#define PERM_OTHER_WRITE  0002
/// @brief Permission d'exécution pour les autres
#define PERM_OTHER_EXEC   0001
/** @} */

