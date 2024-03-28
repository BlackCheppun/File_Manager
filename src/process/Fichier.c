#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "process/Fichier.h"
#include "data/TypeDossier.h"
#include "util/constant.h"
#include "util/Tools.h"
#include "data/TypeSuperBloc.h"

File* myOpen(char* fileName){
    SuperBlock sb;
    BlockBitmap bbmp;
    File* array = NULL;

    loadSuperBlock(&sb);
    loadBlockBitmap(&bbmp);
    loadFileBlock(&array);
    
    printSB(sb);
    printBBMP(bbmp);
    printFILE(array);

    return NULL;
}


int myClose(int file){
    return close(file);
}

/**
 * Lit un certain nombre d'octets à partir de la position actuelle du pointeur de lecture dans un fichier spécifié.
 *
 * @param f Un pointeur vers la structure de fichier (File) à partir duquel lire les données.
 * @param buffer Un pointeur vers un tampon (buffer) où stocker les données lues.
 * @param nBytes Le nombre d'octets à lire à partir du fichier.
 * @return Le nombre d'octets effectivement lus depuis le fichier. En cas d'erreur, retourne -1.
 */
int myRead(File* f, void* buffer, int nBytes) {
    int fd = open(PARTITION_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    if (lseek(fd, f->posSeek, SEEK_SET) == -1) {
        perror("Erreur lors du déplacement du pointeur de lecture");
        close(fd);
        return -1;
    }

    int bytesRead = read(fd, buffer, nBytes);
    if (bytesRead == -1) {
        perror("Erreur lors de la lecture du fichier");
        close(fd);
        return -1;
    }

    close(fd);
    return bytesRead;
}

/**
 * Déplace le pointeur de lecture/écriture dans un fichier spécifié en fonction d'un décalage et d'une base donnée.
 *
 * @param f Un pointeur vers la structure de fichier (File) dans laquelle déplacer le pointeur de lecture/écriture.
 * @param offset Le décalage (offset) à appliquer au pointeur de lecture/écriture.
 * @param base La base à utiliser pour interpréter le décalage (offset). Les valeurs possibles sont SEEK_SET, SEEK_CUR et SEEK_END.
 */
void mySeek(File* f, int offset, int base) {
    int fd = open(PARTITION_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    int whence;
    switch (base) {
        case SEEK_SET:
            whence = SEEK_SET;
            break;
        case SEEK_CUR:
            whence = SEEK_CUR;
            break;
        case SEEK_END:
            whence = SEEK_END;
            break;
        default:
            fprintf(stderr, "Base de recherche invalide\n");
            close(fd);
            return;
    }

    f->posSeek = lseek(fd, offset, whence);
    if (f->posSeek == -1) {
        perror("Erreur lors du déplacement du pointeur de lecture/écriture");
        close(fd);
        return;
    }

    close(fd);
}
