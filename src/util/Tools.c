#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "util/Tools.h"
#include "data/TypeSuperBloc.h"
#include "data/TypeFichier.h"
#include "data/TypeDossier.h"
#include "data/TypeBitmap.h"


/**
 * @brief Retourne l'indice du premier bloc vide
 * 
 * @param bbmp 
 * @return int i s'il existe | -1 si non trouvé
 */
int indexOfFreeBBMP(BlockBitmap bbmp){
    int i= 0;
    while (i< BLOCK_BITMAP_ARRAY_SIZE && bbmp.bmpTab[i] != 0)
    {
        i++;
    }
    if (i >= BLOCK_BITMAP_ARRAY_SIZE)
    {
        return -1;
    }
    return i;
}

int indexBBMPOfPosSeek(File* file){
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    return indexBBMPOfPosSeekLoaded(file,bbmp);
}

int indexBBMPOfPosSeekLoaded(File* file,BlockBitmap bbmp){
    if (file->posSeek > file->size)
    {
        perror("cursor cannot exceed file size");
        exit(-1);
    }
    int seekVal = file->posSeek;
    int i = file->posInBlockBMP;
    while (seekVal > BLOCK_SIZE)
    {
        seekVal = seekVal - BLOCK_SIZE;
        i = bbmp.bmpTab[i];
    }
    return i;
}


int saveFileBlock(File f,int index){
    int fd = open(PARTITION_NAME,O_RDWR);
    unsigned int tmp = f.posSeek;
    f.posSeek = 0;
    if (fd == -1)
    {
        perror("save failed FileBlock");
        return -1;
    }
    if (lseek(fd,FILEBLOCK_OFFSET + index*sizeof(File),SEEK_SET) == -1){
        close(fd);
        perror("save seek failed FileBlock");
        return -1;
    }
    if (write(fd,&f,sizeof(f)) == -1){
        close(fd);
        perror("save write failed FileBlock");
        return -1;
    }
    f.posSeek = tmp;
    close(fd);
    return 0;
}

int saveSuperBlock(SuperBlock sb){
    int fd = open(PARTITION_NAME,O_RDWR);
    if (fd == -1)
    {
        perror("save failed SuperBlock");
        return -1;
    }

    if (write(fd,&sb,sizeof(sb)) == -1){
        close(fd);
        perror("save write failed SuperBlock");
        return -1;
    }
    
    close(fd);
    return 0;
}

int saveBBMP(BlockBitmap bbmp){
    int fd = open(PARTITION_NAME,O_RDWR);
    if (fd == -1)
    {
        perror("save failed FileBlock");
        return -1;
    }
    if (lseek(fd,BITMAPBLOCK_OFFSET,SEEK_SET) == -1){
        close(fd);
        perror("save seek failed FileBlock");
        return -1;
    }
    if (write(fd,&bbmp,sizeof(bbmp)) == -1){
        close(fd);
        perror("save write failed FileBlock");
        return -1;
    }
    
    close(fd);
    return 0;
}

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
int loadFileBlock(File *fileArray){

    int fd = open(PARTITION_NAME, O_RDONLY);
    
    if (fd == -1)
    {
        perror("erreur lecture SuperBlock");
        return -1;
    }

    if (lseek(fd,FILEBLOCK_OFFSET,SEEK_SET) == -1){
        close(fd);
        perror("error seek load Fileblock");
        return -1;
    }

    for (int i = 0; i < NUMBER_OF_BLOCK; i++){
        File tmpF;
        read(fd,&tmpF,sizeof(File));
        fileArray[i] = tmpF;
    }

    close(fd);
    return 0;
}
int loadDirBlock(Directory* dirArray){
    int fd = open(PARTITION_NAME, O_RDONLY);
    
    if (fd == -1)
    {
        perror("erreur lecture SuperBlock");
        return -1;
    }

    if (lseek(fd,DIRBLOCK_OFFSET,SEEK_SET) == -1){
        close(fd);
        perror("error seek load Fileblock");
        return -1;
    }

    for (int i = 0; i < MAX_DIR_AMOUNT; i++){
        Directory tmpD;
        read(fd,&tmpD,sizeof(Directory));
        dirArray[i] = tmpD;
    }

    close(fd);
    return 0;
}

void printSB(SuperBlock sb){
    printf("SuperBlock infos :\n");
    printf("\tTotal blocks : %u\n",sb.totalBlock);
    printf("\tSize of block : %u\n",sb.tailleBlock);
    printf("\tRemaining blocks : %u\n",sb.nbBlockDispo);
    printf("\tTotal files : %u\n",sb.totalFile);
    printf("\tRemaining files : %u\n",sb.nbFileDispo);
    printf("\tTotal directories : %u\n",sb.totalDirectory);
    printf("\tRemaining directories : %u\n",sb.nbDirectoryDispo);
    printf("\tProprietary file system : %c%c%c\n",sb.fs_name[0],sb.fs_name[1],sb.fs_name[2]);
}
void printBBMP(BlockBitmap bbmp){
    printf("BlockBitmap infos :\n");
    int cpt = 0;
    printf("L%d:\t",cpt++);
    for (int i = 0; i < BLOCK_BITMAP_ARRAY_SIZE; i++)
    {
        printf("'%u'",bbmp.bmpTab[i]);
        if ((i+1)%(BLOCK_BITMAP_ARRAY_SIZE/64) == 0 && cpt != 64)printf("\nL%d:\t",cpt++);
    }
    printf("\n");
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
    printf("FileBlock infos :\n");
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        printf("\tD%d:\t",i);
        printf("'%s',Entries %u, Parent Index %u\n",array[i].nomDossier,array[i].nbEntry,array[i].parentIndex);
        printDirEnt(array[i].dirEnt,array[i].nbEntry);
    }
}

void printDirEnt(DirectoryEntry array[], int nbDirEnt){
    for (int i = 0; i < nbDirEnt; i++)
    {
        printf("\tisDir:%u, index:%u",(int)array[i].isDirectory,array[i].index);
    }
    printf("\n");
}
