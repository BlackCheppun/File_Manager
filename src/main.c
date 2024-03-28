#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "process/Fichier.h"
#include "process/Partition.h"
#include "data/TypeSuperBloc.h"
#include "data/TypeDossier.h"
#include "util/constant.h"
#include "util/Tools.h"

char* PARTITION_NAME;

int main(int argc, char** argv){
    char* nom = "mapartition.bin";
    PARTITION_NAME = nom;
    // myFormat(nom);
    printf("nb octet pour offset : %d\n",BLOCK_BITMAP_SIZE);
    printf("taille de superblock : %d\n",sizeof(SuperBlock));
    printf("taille de directory : %d\n",sizeof(Directory));
    printf("taille totale partition : %d\n",PARTITION_SIZE);
    printf("taille partie reserve partition : %d\n",DATABLOCK_OFFSET);
    printf("taille partie libre partition : %d\n",PARTITION_SIZE-DATABLOCK_OFFSET);
    printf("%%used par reserve partition : %.3f%%\n", (float)DATABLOCK_OFFSET/(float)PARTITION_SIZE);
    myOpen("coucou"); 
    
    return 0;
   
}