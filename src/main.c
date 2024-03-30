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
    myFormat(nom);
    
    // printf("nb octet pour offset : %ld\n",BLOCK_BITMAP_SIZE);
    // printf("taille de superblock : %ld\n",sizeof(SuperBlock));
    // printf("taille de directory : %ld\n",sizeof(Directory));
    // printf("taille totale partition : %ld\n",PARTITION_SIZE);
    // printf("taille partie reserve partition : %ld\n",DATABLOCK_OFFSET);
    // printf("taille partie libre partition : %ld\n",PARTITION_SIZE-DATABLOCK_OFFSET);
    // printf("%%used par reserve partition : %.3f%%\n", (float)DATABLOCK_OFFSET/(float)PARTITION_SIZE);

    File* f = myOpen("coucou");
    
    if (f != NULL)
    {
        char* msg = "je suis un bout de texte!";
        myWrite(f,msg,strlen(msg));
        mySeek(f,-10,MYSEEK_CUR);  
        char* msg1 = "abcdefghijklmnopqrstuvwxyz";
        myWrite(f,msg1,strlen(msg1));
    }
    
    
    File* file = myOpen("papi");  
    if (file == NULL){
        printf("Je suis NULL\n");
    }else{
        char* bigmsg = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque eget risus quam. Sed sed tortor sit amet velit condimentum condimentum. Nunc lorem orci, ullamcorper id rhoncus nec, vehicula id risus. Sed iaculis neque maximus mauris lacinia hendrerit. Vestibulum tincidunt eget nunc vitae consectetur. Fusce aliquam ex ac mauris suscipit consectetur. Pellentesque arcu sem, mattis et commodo quis, molestie id arcu. Sed accumsan porttitor justo, sed luctus enim fermentum tempus. Maecenas interdum enim erat, a blandit nulla facilisis nec. Integer malesuada turpis massa, eget varius lacus viverra sed. Curabitur ultrices felis ut odio elementum pulvinar. Aenean sed sodales velit. Cras vitae pellentesque urna.Nam non elit in purus tristique viverra sit amet non ipsum. Sed a neque eget nibh sagittis euismod. Donec sollicitudin porta sapien. Morbi blandit a magna vitae eleifend. Donec pharetra hendrerit diam, ac consequat metus rutrum pharetra. Etiam non magna erat. Suspendisse sit amet suscipit dolor. In semper, libero id porta bibendum, justo augue sagittis mi, vel consectetur metus nunc quis ex. Ut ex tortor, vestibulum vitae libero eu, iaculis sodales erat. Suspendisse dignissim, justo ut blandit suscipit, est lacus finibus magna, in efficitur arcu nulla in sapien. Nunc non convallis tellus, et ullamcorper purus. Curabitur nec dapibus lectus. Integer tincidunt, libero id tristique scelerisque, purus nunc laoreet odio, ut congue risus ipsum non dolor. Nullam ornare enim id nisi porttitor, sit amet maximus dui ullamcorper.Curabitur a odio a nibh bibendum porttitor. Nullam in enim fermentum, eleifend magna ultricies, ornare ligula. Nunc imperdiet tincidunt velit nec suscipit. Phasellus blandit vehicula enim, non blandit sem sollicitudin a. Sed vulputate ipsum mi, vel aliquam risus feugiat sed. Nullam ex massa, laoreet et ultrices at, cursus id turpis. Phasellus magna enim, efficitur sit amet aliquam vel, feugiat quis arcu. Morbi sed elit vel nulla blandit pretium in sed elit. Sed faucibus interdum posuere.Nam posuere eros at dui malesuada, ut sollicitudin turpis luctus. Pellentesque ac est eget ante imperdiet rhoncus non viverra mauris. Vestibulum libero ligula, aliquet eget euismod at, mollis id massa. Nullam pretium imperdiet lacus, sit amet sagittis dolor semper fringilla. Curabitur vehicula convallis lacinia. Suspendisse dui nibh, placerat ut ultrices nec, elementum id augue. Sed purus ipsum, maximus id quam eu, scelerisque imperdiet ipsum. Mauris sollicitudin efficitur tincidunt. Phasellus sagittis rhoncus lectus eget varius. Quisque magna lacus, sagittis finibus lorem nec, fringilla faucibus erat. Aliquam quis tempor enim.Etiam imperdiet ultrices ligula, at egestas magna tincidunt vitae. Maecenas in convallis libero. Vivamus in lectus id sem eleifend tincidunt. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Curabitur euismod sed sem sit amet semper. Nullam et ante facilisis, malesuada purus et, finibus libero. Curabitur eget nisi luctus, pretium urna sit amet, pretium sem. Nunc ut ultrices ipsum, non ultricies nibh. In risus velit, mattis a rhoncus id, dictum in ipsum. Phasellus ac ultrices libero, eget tempus sapien.Pellentesque sit amet justo in urna dignissim malesuada rutrum id risus. Nam scelerisque lacus eget dolor iaculis, nec commodo sem laoreet. Donec quis luctus magna, in posuere sapien. Vivamus in ipsum sagittis, pretium leo sit amet, posuere libero. Mauris tincidunt felis purus, et eleifend est dapibus in. Maecenas semper vitae turpis ac gravida. Nullam leo neque, ornare eget ligula ut, tempor tristique nisl. Praesent iaculis vitae libero sit amet vestibulum.Donec non aliquet erat. Duis mattis magna in lectus aliquam, in elementum est fermentum. Nullam sollicitudin, leo a lacinia tempor, erat turpis pharetra orci, eu accumsan mauris velit quis enim. Fusce et leo nec massa lobortis convallis eget sed sem. Vestibulum tincidunt enim pretium, blandit nulla at, fringilla metus. Curabitur vitae elit sit amet augue mollis eleifend. Ut blandit convallis tellus non odio.NEWBLOCKHERE";
        myWrite(file,bigmsg,strlen(bigmsg));
        myWrite(file,bigmsg,strlen(bigmsg));
        printf("je mappele : %s \n",file->nom);
        printf("taille file: %u \n",file->size);
        printf("posBMP : %u \n",file->posInBlockBMP);
        printf("seek : %u \n",file->posSeek);
        myRename("papi","mamie");
    }
    
    return 0;
}