/**
 * @file Fichier.h
 * @brief Permet la manipulation de File
 */
#ifndef FICHIER_H
#define FICHIER_H

#include "../data/TypeFichier.h"
#include "../data/TypeDossier.h"
/**
 * @brief Calcul par rapport au début du fichier
 */
#define MYSEEK_START 0
/**
 * @brief Calcul par rapport à la position actuelle
 */
#define MYSEEK_CUR 1
/**
 * @brief Calcul par rapport à la fin
 */
#define MYSEEK_END 2

/**
 * @brief Ouvre (ou créer) le fichier fileName dans notre partition
 *
 * @param fileName Nom du fichier
 * @return File* du fichier fileName | NULL si échec
 */
File *myOpen(char *fileName, short dirID);

/**
 * Lit un certain nombre d'octets à partir de la position actuelle du pointeur de lecture dans un fichier spécifié.
 *
 * @param f Un pointeur vers la structure de fichier (File) à partir duquel lire les données.
 * @param buffer Un pointeur vers un tampon (buffer) où stocker les données lues.
 * @param nBytes Le nombre d'octets à lire à partir du fichier.
 * @return Le nombre d'octets effectivement lus depuis le fichier. En cas d'erreur, retourne -1.
 */
int myRead(File *f, void *buffer, int nBytes);

/**
 * @brief
 *
 * @param f
 * @param buffer
 * @param nBytes
 * @return int
 */
int myWrite(File *f, void *buffer, int nBytes);

/**
 * Déplace le pointeur de lecture/écriture dans un fichier spécifié en fonction d'un décalage et d'une base donnée.
 * <p>Attention seul MYSEEK_CUR autorise le déplacement vers l'avant sans provoquer d'erreur (c'est à dire que le fichier grandit)</p>
 * @param f Un pointeur vers la structure de fichier (File) dans laquelle déplacer le pointeur de lecture/écriture.
 * @param offset Le décalage (offset) à appliquer au pointeur de lecture/écriture.
 * @param base La base à utiliser pour interpréter le décalage (offset). Les valeurs possibles sont MYSEEK_START, MYSEEK_CUR et MYSEEK_END.
 */
void mySeek(File *f, int offset, int base);

/**
 * @brief Retourne la taille du fichier qu'il fait actuellement
 *
 * @param f File
 * @return unsigned int taille en Octet
 */
unsigned int mySize(File *f);

/**
 * @brief Retourne la position de la tête du curseur du fichier
 *
 * @param f File
 * @return unsigned int position du curseur (entre 0 à size)
 */
unsigned int myTell(File *f);

/**
 * @brief Renomme le fichier oldName par son newName <br/>
 * Attention, case sensitive ("abc" != "aBc")
 *
 * @param oldName nom du fichier
 * @param newName son nouveau nom
 * @return int 0 en cas de réussite | -1 échec
 */
int myRename(char *oldName, char *newName);

/**
 * @brief Efface le fichier fileName et réorganise la zone réservée
 * <br/>
 * <strong>Attention il ne supprime pas de manière sécurisé (vos données sont encores présentes dans la partition)</strong>
 * @param fileName nom du fichier à supprimer
 * @return int 0 eb cas de réussite | -1 échec
 */
int myDelete(char *fileName);

/**
 * @brief génère un ID unique pour le répertoire
 * @param void
 *
 * @return short ID du répertoire
 */
short generateUniqueRepoID(void);

/**
 * @brief Sauvegarde le tableau de fichiers dans le block bitmap
 *
 *
 * @param dirArray
 * @return int 0 en cas de réussite | -1 échec
 */
int saveDirBlock(Directory *dirArray);

/**
 * @brief Ferme le fichier.
 *
 * @param File* file Pointeur vers la structure de fichier à fermer.
 * @return int 0 en cas de réussite | -1 échec
 */
int myClose(File *file);

/**
 * @brief Creates a symbolic link from linkName to targetPath
 * 
 * @param linkName Name of the symbolic link to create
 * @param targetPath Path to the target file/directory
 * @param dirID ID of the directory where to create the link
 * @return int 0 on success, -1 on failure
 */
int myCreateSymbolicLink(char *linkName, char *targetPath, short dirID);

/**
 * @brief Creates a hard link from linkName to targetFile
 * 
 * @param linkName Name of the hard link to create
 * @param targetFile Name of the target file (hard links can only link to files)
 * @param dirID ID of the directory where to create the link
 * @return int 0 on success, -1 on failure
 */
int myCreateHardLink(char *linkName, char *targetFile, short dirID);

/**
 * @brief Resolves a symbolic link to get the actual target path
 * 
 * @param linkName Name of the symbolic link to resolve
 * @param dirID ID of the directory containing the link
 * @return char* Resolved path or NULL if error
 */
char* myReadLink(char *linkName, short dirID);

#endif