#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "process/Fichier.h"
#include "data/TypeDossier.h"
#include "util/constant.h"
#include "util/Tools.h"
#include "data/TypeSuperBloc.h"

/**
 * @brief Ouvre (ou créer) le fichier fileName dans notre partition
 * 
 * @param fileName Nom du fichier
 * @return File* du fichier fileName | NULL si échec
 */
File* myOpen(char* fileName){
    SuperBlock sb;
    loadSuperBlock(&sb);
    File array[NUMBER_OF_BLOCK];
    loadFileBlock(array);
    int i = 0;
    int nbActuelFile = sb.totalFile - sb.nbFileDispo;

    /* Dans le cas ou le fichier existe*/
    while (i<nbActuelFile && strcmp(array[i].nom,fileName) != 0 )
    {
        i++;
    }
    if (i<nbActuelFile)
    {
        File* found = &(array[i]);
        return found;
    }

    /* Il n'existe pas il faut le créer*/
    BlockBitmap bbmp;
    Directory arrayDir[MAX_DIR_AMOUNT];
    loadBlockBitmap(&bbmp);
    loadDirBlock(arrayDir);
    // verifie si on a encore de la place libre
    int index = indexOfFreeBBMP(bbmp);
    if (index == -1)
    {
        perror("Cannot find free space");
        return NULL;
    }
    // on attribue la case au fichier avec FF
    bbmp.bmpTab[index] = USHRT_MAX; 
    int fd = open(PARTITION_NAME,O_RDWR);
    if (fd == -1)
    {
        perror("can't open partition myOpen");
        return NULL;
    }
    if (lseek(fd,BITMAPBLOCK_OFFSET + index * sizeof(unsigned short),SEEK_SET) == -1)
    {
        close(fd);
        perror("can't seek to indexBBMP myOpen");
        return NULL;
    }
    unsigned short max = USHRT_MAX;
    if (write(fd,&max,sizeof(max)) == -1){
        close(fd);
        perror("can't change value BBMP myOpen");
        return NULL;
    }
    close(fd);


    int indFile = NUMBER_OF_BLOCK - sb.nbFileDispo;
    sb.nbFileDispo--;
    File* tmp = (File*) malloc(sizeof(File));
    strcpy(tmp->nom,fileName);
    for (int i = strlen(tmp->nom); i < MAX_FILES_NAME_SIZE; i++)
    {
        tmp->nom[i] = '\0';
    }
    tmp->posInBlockBMP = index;
    tmp->size = 0;
    tmp->posSeek = 0;

    // persistance des données
    savetoBitmapBlock(*tmp,indFile);
    saveSuperBlock(sb);

    /*
    Step 1 check if fileName exist in the fileBlock
        If yes return the file pointer
        else find the first index in the BBMP where it is empty
        assign max unsigned value (limit.h)

    Step 2 only if in case we file don't exist
        Trouver l'indice du tableau libre,
        Affecter USHORT_MAX a celui ci => BBMP OFFSET + index*sizeof(),
        write a cette endroit la le BBMP
        trouver l'indice du tableau FILES : NBfile - sb.nbFileDispo
        attribuer dans la liste des files size = 0,
    */ 

    return tmp;
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
