#ifndef FICHIER_H
#define FICHIER_H

#include "data/TypeFichier.h"

#define MYSEEK_START 0
#define MYSEEK_CUR 1
#define MYSEEK_END 2

File* myOpen(char* fileName);
int myRead(File* f, void* buffer,int nBytes);
int myWrite(File* f, void* buffer,int nBytes);
void mySeek(File* f,int offset,int base);

#endif