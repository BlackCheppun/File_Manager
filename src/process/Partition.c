#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "util/constant.h"
#include "data/TypeFichier.h"
#include "data/TypeDossier.h"
#include "process/Partition.h"

int myFormat(char* nomPartition){
    int fd = open(nomPartition,O_CREAT|O_RDWR,0644);
    
    if (fd == -1)
    {
        perror("probleme lors de la creation partition");
        return -1;
    }
    
    off_t newPosition = lseek(fd, PARTITION_SIZE-1, SEEK_SET);

    if (newPosition == -1) {
        perror("erreur seek partition");
        close(fd);
        return -1;
    }

    char nullChar = '\0';
    ssize_t bytesWritten = write(fd, &nullChar, sizeof(char));
    
    if (bytesWritten == -1) {
        perror("erreur allocation memoire partition");
        close(fd);
        return -1;
    }

    int nbWrite;
    // Superblock information writing
    lseek(fd,0,SEEK_SET);
    unsigned short partitionInfo[7] = {NUMBER_OF_BLOCK,BLOCK_SIZE,NUMBER_OF_BLOCK,NUMBER_OF_BLOCK,NUMBER_OF_BLOCK,MAX_DIR_AMOUNT,MAX_DIR_AMOUNT};
    nbWrite = write(fd,partitionInfo,sizeof(partitionInfo));
    char* fs_proprietary = "ALS"; 
    nbWrite = write(fd,fs_proprietary,sizeof(fs_proprietary));

    
    // BlockBitmap writing
    unsigned short zeroVal = 0;
    for (int i = 0; i < BLOCK_BITMAP_SIZE; i++)
    {
        nbWrite = write(fd,&zeroVal,sizeof(unsigned short));
        if (nbWrite <= 0)
        {
            perror("erreur lors de l'init du block bitmap");
            close(fd);
            return -1;
        }
    }
    
    
    // File Block writing
    for (int i = 0; i < FILE_BLOCK_SIZE; i++)
    {
        nbWrite = write(fd,&nullChar, sizeof(unsigned short));
        if (nbWrite <= 0)
        {
            perror("erreur lors de l'init du File block");
            close(fd);
            return -1;
        }
    }
    

    // Dir block writing
    for (int i = 0; i < DIR_BLOCK_SIZE; i++)
    {
        nbWrite = write(fd,&nullChar,1);
        if (nbWrite <= 0)
        {
            perror("erreur lors de l'init du Dir block");
            close(fd);
            return -1;
        }
    }

    close(fd);
    return 0;
}