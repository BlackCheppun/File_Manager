#include "Fichier.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int myFormat(char* nomPartition){
    int fd = open(nomPartition,O_CREAT|O_RDWR,0644);
    if (fd == -1)
    {
        perror("probleme lors de la creation partition");
        return -1;
    }
    close(fd);
    return 0;
}


int myClose(int file){
    return close(file);
}