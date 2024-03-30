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
        File* found = (File*) malloc(sizeof(File));
        strcpy(found->nom, array[i].nom);
        found->posInBlockBMP = array[i].posInBlockBMP;
        found->posSeek = array[i].posSeek;
        found->size = array[i].size;
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
    saveFileBlock(*tmp,indFile);
    saveSuperBlock(sb);

    return tmp;
}

int myWrite(File* f, void* buffer,int nBytes){
    SuperBlock sb;
    loadSuperBlock(&sb);
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    unsigned int X = f->posSeek + (unsigned int) nBytes;
    /*
        Block = 512
        Size = 330
        Seek = 300
        Msg = 810
        Cb de bloc ?
        X = Seek + Msg : 1110
        X < Size ? Pas bsoin de bloc
        X > Size ? 
            (X - Size)(770)/Block(512) = 1.4 => 2 bloc en plus
    */
    unsigned int nbBlocNeed = 0;
    if (X > f->size)
    {
        nbBlocNeed = ((int)(X-f->size) / BLOCK_SIZE) + (((int)(X-f->size) % BLOCK_SIZE) != 0);
    }
    // calcul de bloc dispo
    if (sb.nbBlockDispo < nbBlocNeed)
    {
        perror("Not enough block myWrite");
        return -1;
    }
    sb.nbBlockDispo -= nbBlocNeed;

    // deplacement initial vers la tete du curseur
    int currentIndex = indexBBMPOfPosSeekLoaded(f,bbmp);
    // bbmp.bmpTab[currentIndex] = USHRT_MAX;
    int toWrite = nBytes;

    // début de l'écriture 
    int fd = open(PARTITION_NAME,O_RDWR);
    if (fd == -1)
    {
        perror("couldn't open partition myWrite");
        return -1;
    }
    int offset=0, written, nbWrite, iFreeBlock;
    // Comme pour write() on écrit à partir de la posSeek
    if (lseek(fd,(currentIndex*BLOCK_SIZE) + DATABLOCK_OFFSET + f->posSeek%BLOCK_SIZE,SEEK_SET) == -1)
    {
        close(fd);
        perror("error initial seek myWrite");
        return -1;
    }
    
    // save de la data dans le DataBlock
    while (toWrite > 0)
    {
        if (toWrite < BLOCK_SIZE){
            written = toWrite;
        }else{
            written = BLOCK_SIZE;
        }
        if ((nbWrite = write(fd,(char*)(buffer) + offset,written)) == -1)
        {
            close(fd);
            perror("error write myWrite");
            return -1;
        }
        toWrite -= nbWrite;
        offset += nbWrite;
        if (toWrite > 0)
        {
            iFreeBlock = indexOfFreeBBMP(bbmp);
            bbmp.bmpTab[currentIndex] = iFreeBlock;
            currentIndex = iFreeBlock;
        }
        if (lseek(fd,(currentIndex*BLOCK_SIZE) + DATABLOCK_OFFSET,SEEK_SET) == -1)
        {
            close(fd);
            perror("error seek myWrite");
            return -1;
        }
    }
    if (bbmp.bmpTab[currentIndex] == 0){
        bbmp.bmpTab[currentIndex] = USHRT_MAX;
    }
    if (X > f->size)
    {
        f->size = (unsigned int) X;
    }
    f->posSeek = f->posSeek + (unsigned int)nBytes;
    close(fd);
    // Superblock saved
    saveSuperBlock(sb);
    // BitmapBlock saved
    saveBBMP(bbmp);
    // FileBlock saved
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);
    int i = 0;
    int nbActuelFile = sb.totalFile - sb.nbFileDispo;
    while (i < nbActuelFile && strcmp(fileArray[i].nom,f->nom)!=0)
    {
        i++;
    }
    saveFileBlock(*f,i);
    return nBytes;
}



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

void mySeek(File* f, int offset, int base) {
    int currentPos = (int)f->posSeek;
    int sizeFile = (int)f->size;
    switch (base) {
        case MYSEEK_START:
            if (offset < 0){
                perror("seek negative from start mySeek");
                break;
            }
            if (offset > sizeFile){
                perror("seek outofbound from start mySeek");
                break;
            }
            f->posSeek = offset;
            break;
        case MYSEEK_CUR:
            if ((currentPos + offset) < 0)
            {
                perror("seek outofbound from current mySeek");
                break;
            }

            if ((currentPos + offset) > sizeFile)
            {
                int nbEmpty = (currentPos + offset) - sizeFile;
                f->posSeek = f->size ;
                char * emptyNull = (char*) malloc((nbEmpty+1) * sizeof(char));
                for (int i = 0; i < nbEmpty; i++)
                {
                    emptyNull[i] = '\0';
                }
                myWrite(f,emptyNull,nbEmpty);
                break;
            }
            
            f->posSeek += (unsigned int) offset;
            break;
        case MYSEEK_END:
            if (offset < 0)
            {
                perror("seek negative from end mySeek");
                break;
            }
            if (offset > sizeFile)
            {
                perror("seek outofbound from end mySeek");
                break;
            }
            f->posSeek = sizeFile - offset;
            break;
        default:
            perror("Unknown base mySeek");
            return;
    }
}


unsigned int mySize(File* f){
    return f->size;
}
unsigned int myTell(File* f){
    return f->posSeek;
}
int myRename(char* oldName, char* newName){
    if (strlen(newName) > MAX_FILES_NAME_SIZE)
    {
        perror("New name exceeded size limit myRename");
        return -1;
    }
    SuperBlock sb;
    loadSuperBlock(&sb);
    File array[NUMBER_OF_BLOCK];
    loadFileBlock(array);
    int i = 0;
    int nbActuel = sb.totalFile - sb.nbFileDispo;
    while (i < nbActuel && strcmp(oldName,array[i].nom) != 0)
    {
        i++;
    }
    if (i < nbActuel)
    {
        File tmp = array[i];
        for (int j = 0; j < strlen(newName); j++)
        {
            tmp.nom[j] = newName[j];
        }
        for (int j = strlen(newName); j < MAX_FILES_NAME_SIZE-1; j++)
        {
            tmp.nom[j] = '\0';
        }
        printf("new name %s\n",tmp.nom);
        saveFileBlock(tmp,i);
        return 0;
    }
    perror("File not found myRename");
    return -1;
}
