#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "process/Fichier.h"
#include "data/TypeDossier.h"
#include "util/constant.h"



int myClose(int file){
    return close(file);
}