#ifndef TYPE_SUPERBLOC_H
#define TYPE_SUPERBLOC_H

typedef struct sb{
    unsigned short totalBlock;
    unsigned short tailleBlock;
    unsigned short nbBlockDispo;
    unsigned short totalFile;
    unsigned short nbFileDispo;
    unsigned short totalDirectory;
    unsigned short nbDirectoryDispo;
    char fs_name[4];
} SuperBlock;

#endif