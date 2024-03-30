/**
 * @file Fichier.h
 * @author Farah ALIANE
 * @author Laurent LIN
 * @brief Permet la manipulation de File
 */
#ifndef FICHIER_H
#define FICHIER_H

#include "data/TypeFichier.h"
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
File* myOpen(char* fileName);

/**
 * Lit un certain nombre d'octets à partir de la position actuelle du pointeur de lecture dans un fichier spécifié.
 *
 * @param f Un pointeur vers la structure de fichier (File) à partir duquel lire les données.
 * @param buffer Un pointeur vers un tampon (buffer) où stocker les données lues.
 * @param nBytes Le nombre d'octets à lire à partir du fichier.
 * @return Le nombre d'octets effectivement lus depuis le fichier. En cas d'erreur, retourne -1.
 */
int myRead(File* f, void* buffer,int nBytes);

int myWrite(File* f, void* buffer,int nBytes);

/**
 * Déplace le pointeur de lecture/écriture dans un fichier spécifié en fonction d'un décalage et d'une base donnée.
 * <p>Attention seul MYSEEK_CUR autorise le déplacement vers l'avant sans provoquer d'erreur (c'est à dire que le fichier grandit)</p>
 * @param f Un pointeur vers la structure de fichier (File) dans laquelle déplacer le pointeur de lecture/écriture.
 * @param offset Le décalage (offset) à appliquer au pointeur de lecture/écriture.
 * @param base La base à utiliser pour interpréter le décalage (offset). Les valeurs possibles sont MYSEEK_START, MYSEEK_CUR et MYSEEK_END.
 */
void mySeek(File* f,int offset,int base);

#endif