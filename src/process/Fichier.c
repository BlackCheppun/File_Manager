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