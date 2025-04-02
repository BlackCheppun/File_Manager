/**
 * @file Tools.h
 */

#ifndef TOOLS_H
#define TOOLS_H
#include "../data/TypeSuperBloc.h"
#include "../data/TypeBitmap.h"
#include "../data/TypeFichier.h"
#include "../data/TypeDossier.h"

/**
 * @brief Charge les informations du SuperBloc depuis PARTITION_NAME
 *
 * @param sb adresse du superbloc
 * @return int 0 en cas de réussite | -1 sinon
 */
int loadSuperBlock(SuperBlock *sb);
/**
 * @brief Charge les informations du BlockBitmap depuis PARTITION_NAME
 *
 * @param bbmp adresse du blockbitmap
 * @return int 0 en cas de réussite | -1 sinon
 */
int loadBlockBitmap(BlockBitmap *bbmp);
/**
 * @brief Charge les informations de FileBlock depuis PARTITION_NAME
 *
 * @param fileArray adresse du tableau de file
 * @return int 0 en cas de réussite | -1 sinon
 */
int loadFileBlock(File *fileArray);
/**
 * @brief Charge les informations de DirBlock depuis PARTITION_NAME
 *
 * @param dirArray adresse du tableau de Dossier
 * @return int 0 en cas de réussite | -1 sinon
 */
int loadDirBlock(Directory *dirArray);

/**
 * @brief Retourne le premier indice du tableau Bitmap qui est libre (pour écrire dessus)
 *
 * @param bbmp BlockBitmap avec toutes les informations
 * @return int i si on trouve un bloc libre | -1 sinon
 */
int indexOfFreeBBMP(BlockBitmap bbmp);

/**
 * @brief Retourne l'indice du tableau indiqué par posSeek du File
 * <br/> Par exemple si on a un fichier de 7000B, et que notre seek vaut 4500 alors,
 * on doit recevoir l'indice qui pointe sur le 2eme bloc utilisé par le fichier
 *
 * @param file
 * @return int i si on trouve | -1 en cas d'erreur
 */
int indexBBMPOfPosSeek(File *file);
/**
 * @brief Retourne l'indice du tableau indiqué par posSeek du File avec le BBMP de chargé
 * <br/> Par exemple si on a un fichier de 7000B, et que notre seek vaut 4500 alors,
 * on doit recevoir l'indice qui pointe sur le 2eme bloc utilisé par le fichier
 *
 * @param file
 * @param bbmp
 * @return int i si on trouve | -1 en cas d'erreur
 */
int indexBBMPOfPosSeekLoaded(File *file, BlockBitmap bbmp);
/**
 * @brief Sauvegarde dans PARTITION_NAME l'information du SuperBlock
 *
 * @param sb
 * @return int 0 en cas de réussite | -1 sinon
 */
int saveSuperBlock(SuperBlock sb);
/**
 * @brief Sauvegarde dans PARTITION_NAME l'information d'un File à sa position précise de FileBlock
 *
 * @param file
 * @param index
 * @return int 0 en cas de réussite | -1 sinon
 */
int saveFileBlock(File file, int index);

/**
 * Saves the directory array back to disk.
 * @param dirArray   Array of directories to save
 * @return           0 on success, -1 on failure
 */
int saveDirBlock(Directory *dirArray);

/**
 * @brief Sauvegarde dans PARTITION_NAME l'information du Bitmap
 *
 * @param bbmp Tableau d'indice des zones
 * @return int 0 en cas de réussite | -1 sinon
 */
int saveBBMP(BlockBitmap bbmp);

/**
 * @brief Affiche les informations du superbloc
 *
 * @param sb SuperBlock
 */
void printSB(SuperBlock sb);
/**
 * @brief Affiche le tableau de BlockBitmap pour montrer les zones occupés et libres
 *
 * @param bbmp BlockBitmap qui est chargé par loadBBMP()
 */
void printBBMP(BlockBitmap bbmp);
/**
 * @brief Affiche les informations de tous les fichiers disponibles
 *
 * @param array Tableau de fichier qu'on charge avec loadFileBlock
 */
void printFILE(File array[]);
/**
 * @brief Affiche le contenu (et de ses enfants) de tous les dossiers disponibles
 *
 * @param array Tableau de dossier
 */
void printDIR(Directory array[]);
/**
 * @brief Affiche les informations des enfants du dossier
 *
 * @param array Tableau d'entrée
 * @param nbDirEnt nombre d'élement
 */
void printDirEnt(DirectoryEntry array[], int nbDirEnt);
#endif