#include <stdio.h>
#include <fcntl.h>
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

    lseek(fd,0,SEEK_SET);
    unsigned short partitionInfo[7] = {NUMBER_OF_BLOCK,BLOCK_SIZE,NUMBER_OF_BLOCK,NUMBER_OF_BLOCK,NUMBER_OF_BLOCK,MAX_DIR_AMOUNT,MAX_DIR_AMOUNT};
    write(fd,partitionInfo,sizeof(partitionInfo));
    char* fs_proprietary = "ALS"; 
    write(fd,fs_proprietary,sizeof(fs_proprietary));

    
    
    lseek(fd,0,SEEK_SET);
    unsigned short readBuffer[3];
    read(fd, readBuffer, sizeof(readBuffer));
    printf("%d,%d,%d\n",readBuffer[0],readBuffer[1],readBuffer[2]);
    char buf[10];
    read(fd,buf,3);
    printf("File System : %s\n",buf);

    close(fd);
    return 0;
}