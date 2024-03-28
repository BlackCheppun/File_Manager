#include "util/Tools.h"
#include "data/TypeSuperBloc.h"
#include "data/TypeFichier.h"
#include "data/TypeDossier.h"
#include "data/TypeBitmap.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int loadSuperBlock(SuperBlock* sb){
    if (sb == NULL){
        sb = (SuperBlock*) malloc(sizeof(SuperBlock));
    }

    int fd = open(PARTITION_NAME, O_RDONLY);
    
    if (fd == -1)
    {
        perror("erreur lecture SuperBlock");
        return -1;
    }
    
    unsigned short sbVal[7];
    
    if((read(fd,sbVal,sizeof(sbVal))) < sizeof(sbVal)){
        close(fd);
        perror("error load value SuperBlock");
        return -1;
    }
    
    sb->totalBlock = sbVal[0];
    sb->tailleBlock = sbVal[1];
    sb->nbBlockDispo = sbVal[2];
    sb->totalFile = sbVal[3];
    sb->nbFileDispo = sbVal[4];
    sb->totalDirectory = sbVal[5];
    sb->nbDirectoryDispo = sbVal[6];
    
    char fs_name[4];
    
    if((read(fd,fs_name,sizeof(fs_name))) < sizeof(fs_name)){
        close(fd);
        perror("error load fs_name SuperBlock");
        return -1;
    }
    
    for (int i = 0; i < 4; i++)
    {
        sb->fs_name[i] = fs_name[i];
    }
    
    close(fd);
    return 0;
}
int loadBlockBitmap(BlockBitmap* bbmp){
    if (bbmp == NULL)
    {
        bbmp = (BlockBitmap*) malloc(sizeof(BlockBitmap));
    }
    
    int fd = open(PARTITION_NAME, O_RDONLY);
    
    if (fd == -1)
    {
        perror("erreur lecture BlockBitmap");
        return -1;
    }
    
    if (lseek(fd,BITMAPBLOCK_OFFSET,SEEK_SET) == -1){
        close(fd);
        perror("erreur seek blockbitmap");
        return -1;
    }

    unsigned short val;
    
    for (int i = 0; i < BLOCK_BITMAP_ARRAY_SIZE; i++)
    {
        if (read(fd,&val,sizeof(val)) == -1){
            close(fd);
            return -1;
        }
        bbmp->bmpTab[i] = val;
    }
    
    close(fd);
    return 0;
}
int loadFileBlock(File* fileArray[]){
    if (fileArray == NULL){
        fileArray = (File**) malloc(NUMBER_OF_BLOCK*sizeof(File*));
        if (fileArray == NULL){
            return -1;
        }
        for (int i = 0; i < NUMBER_OF_BLOCK; i++)
        {
            fileArray[i] = (File*) malloc(sizeof(File));
            if (fileArray[i] == NULL){
                return -1;
            }
        }
    }

    int fd = open(PARTITION_NAME, O_RDONLY);
    
    if (fd == -1)
    {
        perror("erreur lecture SuperBlock");
        return -1;
    }

    File farray[NUMBER_OF_BLOCK];
    if (read(fd,farray,sizeof(farray)) == -1)
    {
        close(fd);
        perror("erreur chargement FileBlock");
        return -1;
    }
    for (int i = 0; i < NUMBER_OF_BLOCK; i++)
    {
        

        for (int j = 0; j < MAX_FILES_NAME_SIZE; j++)
        {
            printf("%c",farray[i].nom[j]);
            fileArray[i]->nom[j] = farray[i].nom[j];
        }
        fileArray[i]->posInBlockBMP = farray[i].posInBlockBMP;
        fileArray[i]->posSeek = farray[i].posSeek;
        fileArray[i]->size = farray[i].size;
    }
    
    close(fd);
    return 0;
}
int loadDirBlock(Directory* dirArray){
    
}

void printSB(SuperBlock sb){
    printf("SuperBlock infos :\n");
    printf("\tNb total de blocs : %u\n",sb.totalBlock);
    printf("\tTaille d'un bloc : %u\n",sb.tailleBlock);
    printf("\tNb blocs dispo : %u\n",sb.nbBlockDispo);
    printf("\tNb total de file : %u\n",sb.totalFile);
    printf("\tNb file dispo : %u\n",sb.nbFileDispo);
    printf("\tNb total de directory : %u\n",sb.totalDirectory);
    printf("\tNb directory dispo : %u\n",sb.nbDirectoryDispo);
    printf("\tPropriete file system : %c%c%c\n",sb.fs_name[0],sb.fs_name[1],sb.fs_name[2]);
}
void printBBMP(BlockBitmap bbmp){
    printf("BlockBitmap infos :\n");
    int cpt = 0;
    printf("L%d:\t",cpt++);
    for (int i = 0; i < BLOCK_BITMAP_ARRAY_SIZE; i++)
    {
        printf("'%u'",bbmp.bmpTab[i]);
        if ((i+1)%(BLOCK_BITMAP_ARRAY_SIZE/64) == 0)printf("\nL%d:\t",cpt++);
    }
    
}
void printFILE(File array[]){
    printf("FileBlock infos :\n");
    for (int i = 0; i < NUMBER_OF_BLOCK; i++)
    {
        printf("\tF%d:\t",i);
        printf("'%s', %uB, Index %u\n",array[i].nom,array[i].size,array[i].posInBlockBMP);
    }
    
}
void printDIR(Directory array[]){
    
}
