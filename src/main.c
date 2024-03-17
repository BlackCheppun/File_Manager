#include <stdio.h>
#include "process/Fichier.h"
#include "process/Partition.h"
#include "data/TypeSuperBloc.h"
#include "data/TypeDossier.h"
#include "util/constant.h"
#include "util/Tools.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char** argv){
    char* nom = "mapartition.bin";
    char* PARTITION_NAME = nom;
    myFormat(nom);
    printf("taille de superblock : %d\n",sizeof(SuperBlock));
    printf("taille de directory : %d\n",sizeof(Directory));
    printf("taille totale partition : %d\n",PARTITION_SIZE);
    printf("taille partie reserve partition : %d\n",DATABLOCK_OFFSET);
    printf("taille partie libre partition : %d\n",PARTITION_SIZE-DATABLOCK_OFFSET);
    printf("%%used par reserve partition : %.3f%%", (float)DATABLOCK_OFFSET/(float)PARTITION_SIZE);
    return 0;
}