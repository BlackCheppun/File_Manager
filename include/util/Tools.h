#ifndef TOOLS_H
#define TOOLS_H
#include "data/TypeSuperBloc.h"
#include "data/TypeBitmap.h"
#include "data/TypeFichier.h"
#include "data/TypeDossier.h"

int loadSuperBlock(SuperBlock* sb);
int loadBlockBitmap(BlockBitmap* bbmp);
int loadFileBlock(File *fileArray);
int loadDirBlock(Directory* dirArray);

void printSB(SuperBlock sb);
void printBBMP(BlockBitmap bbmp);
void printFILE(File array[]);
void printDIR(Directory array[]);
void printDirEnt(DirectoryEntry array[], int nbDirEnt);
#endif