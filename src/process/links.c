#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../include/process/Fichier.h"
#include "../../include/data/TypeDossier.h"
#include "../../include/util/constant.h"
#include "../../include/util/Tools.h"
#include "../../include/data/TypeSuperBloc.h"

int myCreateSymbolicLink(char *linkName, char *targetPath, short dirID) {
    // Validate link name length
    if (strlen(linkName) > MAX_FILES_NAME_SIZE)
    {
        perror("Link name exceeds size limit");
        return -1;
    }

    // Load necessary metadata
    SuperBlock sb;
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);
    Directory dirArray[MAX_DIR_AMOUNT];
    loadDirBlock(dirArray);

    // Check if link name already exists in this directory
    int nbActualFiles = sb.totalFile - sb.nbFileDispo;
    for (int i = 0; i < nbActualFiles; i++)
    {
        if (strcmp(fileArray[i].nom, linkName) == 0 && fileArray[i].parentIndex == dirID)
        {
            perror("Link name already exists");
            return -1;
        }
    }

    // Check directory exists and has space
    int dirIndex = -1;
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == dirID)
        {
            dirIndex = i;
            break;
        }
    }
    if (dirIndex == -1)
    {
        perror("Invalid directory ID");
        return -1;
    }
    if (dirArray[dirIndex].nbFiles >= MAX_ENTRIES_DIR)
    {
        perror("Directory file limit reached");
        return -1;
    }

    // Allocate space in bitmap
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    int freeIndex = indexOfFreeBBMP(bbmp);
    if (freeIndex == -1)
    {
        perror("No free blocks available");
        return -1;
    }

    // Mark block as used
    bbmp.bmpTab[freeIndex] = USHRT_MAX;
    int fd = open(PARTITION_NAME, O_RDWR);
    if (fd == -1)
    {
        perror("Partition access error");
        return -1;
    }

    // Update bitmap on disk
    lseek(fd, BITMAPBLOCK_OFFSET + freeIndex * sizeof(unsigned short), SEEK_SET);
    unsigned short max = USHRT_MAX;
    write(fd, &max, sizeof(max));
    close(fd);

    // Create new link entry
    File newLink;
    strncpy(newLink.nom, linkName, MAX_FILES_NAME_SIZE);
    newLink.posInBlockBMP = freeIndex;
    newLink.size = strlen(targetPath); // Size is the length of the target path
    newLink.posSeek = 0;
    newLink.parentIndex = dirID;
    newLink.linkType = LINK_TYPE_SYMBOLIC;
    strncpy(newLink.targetPath, targetPath, MAX_FILES_NAME_SIZE);

    // Update directory
    dirArray[dirIndex].files[dirArray[dirIndex].nbFiles++] = freeIndex;
    saveDirBlock(dirArray);

    // Update filesystem metadata
    unsigned int fileIndex = NUMBER_OF_BLOCK - sb.nbFileDispo;
    sb.nbFileDispo--;
    sb.nbBlockDispo--;

    saveFileBlock(newLink, fileIndex);
    saveSuperBlock(sb);
    saveBBMP(bbmp);

    return 0;
}

int myCreateHardLink(char *linkName, char *targetFile, short dirID) {
    // Validate link name length
    if (strlen(linkName) > MAX_FILES_NAME_SIZE)
    {
        perror("Link name exceeds size limit");
        return -1;
    }

    // Load necessary metadata
    SuperBlock sb;
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);
    Directory dirArray[MAX_DIR_AMOUNT];
    loadDirBlock(dirArray);

    // Find target file
    File *target = NULL;
    int nbActualFiles = sb.totalFile - sb.nbFileDispo;
    for (int i = 0; i < nbActualFiles; i++)
    {
        if (strcmp(fileArray[i].nom, targetFile) == 0 && fileArray[i].parentIndex == dirID)
        {
            target = &fileArray[i];
            break;
        }
    }
    if (!target)
    {
        perror("Target file not found");
        return -1;
    }

    // Check if link name already exists in this directory
    for (int i = 0; i < nbActualFiles; i++)
    {
        if (strcmp(fileArray[i].nom, linkName) == 0 && fileArray[i].parentIndex == dirID)
        {
            perror("Link name already exists");
            return -1;
        }
    }

    // Check directory exists and has space
    int dirIndex = -1;
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == dirID)
        {
            dirIndex = i;
            break;
        }
    }
    if (dirIndex == -1)
    {
        perror("Invalid directory ID");
        return -1;
    }
    if (dirArray[dirIndex].nbFiles >= MAX_ENTRIES_DIR)
    {
        perror("Directory file limit reached");
        return -1;
    }

    // Create new link entry
    File newLink;
    strncpy(newLink.nom, linkName, MAX_FILES_NAME_SIZE);
    newLink.posInBlockBMP = target->posInBlockBMP; // Share the same block as target
    newLink.size = target->size; // Share the same size as target
    newLink.posSeek = 0;
    newLink.parentIndex = dirID;
    newLink.linkType = LINK_TYPE_HARD;
    newLink.targetFileIndex = target->posInBlockBMP; // Store target's block index

    // Update directory
    dirArray[dirIndex].files[dirArray[dirIndex].nbFiles++] = target->posInBlockBMP;
    saveDirBlock(dirArray);

    // Update filesystem metadata
    unsigned int fileIndex = NUMBER_OF_BLOCK - sb.nbFileDispo;
    sb.nbFileDispo--;

    saveFileBlock(newLink, fileIndex);
    saveSuperBlock(sb);

    return 0;
}

char* myReadLink(char *linkName, short dirID) {
    // Load necessary metadata
    SuperBlock sb;
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);

    // Find the link file
    int nbActualFiles = sb.totalFile - sb.nbFileDispo;
    File *link = NULL;
    for (int i = 0; i < nbActualFiles; i++)
    {
        if (strcmp(fileArray[i].nom, linkName) == 0 && fileArray[i].parentIndex == dirID)
        {
            link = &fileArray[i];
            break;
        }
    }
    if (!link)
    {
        perror("Link not found");
        return NULL;
    }

    // Check if it's a symbolic link
    if (link->linkType != LINK_TYPE_SYMBOLIC)
    {
        perror("Not a symbolic link");
        return NULL;
    }

    // Return a copy of the target path
    char *targetPath = strdup(link->targetPath);
    if (!targetPath)
    {
        perror("Failed to allocate memory for target path");
        return NULL;
    }

    return targetPath;
} 