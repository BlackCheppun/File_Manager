/**
 * @file Tools.h
 * @brief Fonctions utilitaires pour la gestion du système de fichiers
 * @author Massin KHELOUFI (30%) & Ilyes AGHOUILES ( ) & Imane ABDELBASSIR ( )
 * 
 * Ce fichier contient l'ensemble des fonctions utilitaires nécessaires pour
 * manipuler les structures de données du système de fichiers, incluant :
 * - Le chargement et la sauvegarde des structures sur le disque
 * - La recherche et la manipulation des blocs
 * - L'affichage des informations de débogage
 */

 #ifndef TOOLS_H
 #define TOOLS_H
 #include "../data/TypeSuperBloc.h"
 #include "../data/TypeBitmap.h"
 #include "../data/TypeFichier.h"
 #include "../data/TypeDossier.h"
 
 /**
  * @brief Charge le super-bloc depuis la partition
  * @param sb Pointeur vers la structure SuperBlock à remplir
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Lit les informations du super-bloc depuis le fichier PARTITION_NAME
  */
 int loadSuperBlock(SuperBlock *sb);
 
 /**
  * @brief Charge la bitmap des blocs depuis la partition
  * @param bbmp Pointeur vers la structure BlockBitmap à remplir
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Lit la table d'allocation des blocs depuis le fichier PARTITION_NAME
  */
 int loadBlockBitmap(BlockBitmap *bbmp);
 
 /**
  * @brief Charge la table des fichiers depuis la partition
  * @param fileArray Pointeur vers le tableau de structures File à remplir
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Lit les métadonnées de tous les fichiers depuis PARTITION_NAME
  */
 int loadFileBlock(File *fileArray);
 
 /**
  * @brief Charge la table des dossiers depuis la partition
  * @param dirArray Pointeur vers le tableau de structures Directory à remplir
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Lit les métadonnées de tous les dossiers depuis PARTITION_NAME
  */
 int loadDirBlock(Directory *dirArray);
 
 /**
  * @brief Recherche le premier bloc libre dans la bitmap
  * @param bbmp Structure BlockBitmap contenant l'état d'allocation des blocs
  * @return Index du premier bloc libre trouvé, ou -1 si aucun bloc libre
  * @details Parcourt la bitmap pour trouver une entrée avec la valeur 0
  */
 int indexOfFreeBBMP(BlockBitmap bbmp);
 
 /**
  * @brief Trouve le bloc contenant une position donnée dans un fichier
  * @param file Pointeur vers la structure File du fichier
  * @return Index du bloc contenant la position, ou -1 en cas d'erreur
  * @details Calcule le bloc correspondant à la position posSeek dans le fichier
  */
 int indexBBMPOfPosSeek(File *file);
 
 /**
  * @brief Version optimisée de indexBBMPOfPosSeek avec bitmap préchargée
  * @param file Pointeur vers la structure File du fichier
  * @param bbmp Structure BlockBitmap déjà chargée en mémoire
  * @return Index du bloc contenant la position, ou -1 en cas d'erreur
  * @details Évite de recharger la bitmap pour chaque recherche
  */
 int indexBBMPOfPosSeekLoaded(File *file, BlockBitmap bbmp);
 
 /**
  * @brief Sauvegarde le super-bloc sur la partition
  * @param sb Structure SuperBlock à sauvegarder
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Écrit les informations du super-bloc dans PARTITION_NAME
  */
 int saveSuperBlock(SuperBlock sb);
 
 /**
  * @brief Sauvegarde les métadonnées d'un fichier
  * @param file Structure File à sauvegarder
  * @param index Position du fichier dans la table des fichiers
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Écrit les métadonnées du fichier à sa position dans PARTITION_NAME
  */
 int saveFileBlock(File file, int index);
 
 /**
  * @brief Sauvegarde la table des dossiers sur la partition
  * @param dirArray Tableau des structures Directory à sauvegarder
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Écrit toutes les métadonnées des dossiers dans PARTITION_NAME
  */
 int saveDirBlock(Directory *dirArray);
 
 /**
  * @brief Sauvegarde la bitmap des blocs sur la partition
  * @param bbmp Structure BlockBitmap à sauvegarder
  * @return 0 en cas de succès, -1 en cas d'erreur
  * @details Écrit la table d'allocation des blocs dans PARTITION_NAME
  */
 int saveBBMP(BlockBitmap bbmp);
 
 /**
  * @brief Affiche les informations du super-bloc
  * @param sb Structure SuperBlock à afficher
  * @details Affiche l'état général de la partition et ses caractéristiques
  */
 void printSB(SuperBlock sb);
 
 /**
  * @brief Affiche l'état d'allocation des blocs
  * @param bbmp Structure BlockBitmap à afficher
  * @details Montre quels blocs sont libres et lesquels sont utilisés
  */
 void printBBMP(BlockBitmap bbmp);
 
 /**
  * @brief Affiche les informations de tous les fichiers
  * @param array Tableau des structures File à afficher
  * @details Liste les métadonnées de tous les fichiers existants
  */
 void printFILE(File array[]);
 
 /**
  * @brief Affiche les informations de tous les dossiers
  * @param array Tableau des structures Directory à afficher
  * @details Liste les métadonnées de tous les dossiers existants
  */
 void printDIR(Directory array[]);
 
 /**
  * @brief Affiche le contenu d'un dossier
  * @param array Tableau des entrées du dossier
  * @param nbDirEnt Nombre d'entrées dans le dossier
  * @details Liste tous les fichiers et sous-dossiers contenus dans un dossier
  */
 void printDirEnt(DirectoryEntry array[], int nbDirEnt);
 
 /**
  * @brief Recherche un dossier par son identifiant
  * @param repoID Identifiant du dossier recherché
  * @param dirArray Tableau des structures Directory
  * @return Index du dossier dans le tableau, ou -1 si non trouvé
  * @details Parcourt la table des dossiers pour trouver celui avec l'ID spécifié
  */
 int findDirIndex(unsigned short repoID, Directory *dirArray);
 
 #endif