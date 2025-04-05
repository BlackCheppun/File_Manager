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

File *myOpen(char *fileName, short dirID)
{
    // Validate filename length
    if (strlen(fileName) > MAX_FILES_NAME_SIZE)
    {
        perror("Filename exceeds size limit");
        return NULL;
    }

    // Load necessary metadata
    SuperBlock sb;
// At the start of myOpen():
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);  // This should get fresh data including permissions
    Directory dirArray[MAX_DIR_AMOUNT];
    loadDirBlock(dirArray);

    // Check if file already exists in this directory
    int nbActualFiles = sb.totalFile - sb.nbFileDispo;
    File *foundFile = NULL;
    for (int i = 0; i < nbActualFiles; i++)
    {
        if (strcmp(fileArray[i].nom, fileName) == 0 && fileArray[i].parentIndex == dirID)
        {
            foundFile = &fileArray[i];
            break;
        }
    }

    // If file exists, handle it
    if (foundFile)
    {
        if(foundFile->permissions < 0400){
            perror("Permissions denied.");
            return NULL;
        }

        // If it's a symbolic link, resolve it
        if (foundFile->linkType == LINK_TYPE_SYMBOLIC)
        {
            // Recursively open the target file
            return myOpen(foundFile->targetPath, dirID);
        }

        // For regular files and hard links, return a copy
        File *existing = malloc(sizeof(File));
        if (!existing)
            return NULL;

        *existing = *foundFile; // Copy all file data
        return existing;
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
        return NULL;
    }
    if (dirArray[dirIndex].nbFiles >= MAX_ENTRIES_DIR)
    {
        perror("Directory file limit reached");
        return NULL;
    }

    // Allocate space in bitmap
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    int freeIndex = indexOfFreeBBMP(bbmp);
    if (freeIndex == -1)
    {
        perror("No free blocks available");
        return NULL;
    }

    // Mark block as used
    bbmp.bmpTab[freeIndex] = USHRT_MAX;
    int fd = open(PARTITION_NAME, O_RDWR);
    if (fd == -1)
    {
        perror("Partition access error");
        return NULL;
    }

    // Update bitmap on disk
    lseek(fd, BITMAPBLOCK_OFFSET + freeIndex * sizeof(unsigned short), SEEK_SET);
    unsigned short max = USHRT_MAX;
    write(fd, &max, sizeof(max));
    close(fd);

    // Create new file entry
    File *newFile = malloc(sizeof(File));
    if (!newFile)
        return NULL;

    strncpy(newFile->nom, fileName, MAX_FILES_NAME_SIZE);
    newFile->posInBlockBMP = freeIndex;
    newFile->size = 0;
    newFile->posSeek = 0;
    newFile->parentIndex = dirID;
    newFile->linkType = LINK_TYPE_NONE; // New files are not links
    newFile->permissions = 0644;        // Permissions par défaut: rw-r--r--
    // Update directory
    dirArray[dirIndex].files[dirArray[dirIndex].nbFiles++] = freeIndex;
    saveDirBlock(dirArray);

    // Update filesystem metadata
    unsigned int fileIndex = NUMBER_OF_BLOCK - sb.nbFileDispo;
    sb.nbFileDispo--;
    sb.nbBlockDispo--;

    saveFileBlock(*newFile, fileIndex);
    saveSuperBlock(sb);
    saveBBMP(bbmp);

    return newFile;
}

int myWrite(File *f, void *buffer, int nBytes)
{
    printf("DEBUG: Attempting write to file %s with permissions %o\n", f->nom, f->permissions);
    if ((f->permissions & 0200) == 0) {
        perror("Write permission denied");
        return -1;
    }

    SuperBlock sb;
    loadSuperBlock(&sb);
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    unsigned int X = f->posSeek + (unsigned int)nBytes;
    /*
        Block = 512
        Size = 330
        Seek = 330
        Msg = 110
        Cb de bloc ?
        X = Seek + Msg : 440
        X < Size ? Pas bsoin de bloc
        X > Size ?
            (X - Size)(110)/Block(512) = 0.4 => 1 bloc en plus
    */
    unsigned int nbBlocNeed = 0;
    if (X > f->size)
    {
        // nbBlocNeed = ((int)(X-f->size) / BLOCK_SIZE) + (((int)(X-f->size) % BLOCK_SIZE) != 0);
        nbBlocNeed = (X - f->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if ((f->posSeek % BLOCK_SIZE + nBytes) < +BLOCK_SIZE)
        {
            nbBlocNeed--;
        }
    }
    // calcul de bloc dispo
    if (sb.nbBlockDispo < nbBlocNeed)
    {
        perror("Not enough block myWrite");
        return -1;
    }
    sb.nbBlockDispo = sb.nbBlockDispo - nbBlocNeed;

    // deplacement initial vers la tete du curseur
    int currentIndex = indexBBMPOfPosSeekLoaded(f, bbmp);
    // bbmp.bmpTab[currentIndex] = USHRT_MAX;
    int toWrite = nBytes;

    // début de l'écriture
    int fd = open(PARTITION_NAME, O_RDWR);
    if (fd == -1)
    {
        perror("couldn't open partition myWrite");
        return -1;
    }
    int offset = 0, written, nbWrite, iFreeBlock;
    // Comme pour write() on écrit à partir de la posSeek
    if (lseek(fd, (currentIndex * BLOCK_SIZE) + DATABLOCK_OFFSET + f->posSeek % BLOCK_SIZE, SEEK_SET) == -1)
    {
        close(fd);
        perror("error initial seek myWrite");
        return -1;
    }

    /* Imaginons que je suis a posSeek = 3000;
    et si il me reste 4200 byte a ecrire alors d'abord
    je dois ecrire 1096 byte pour remplir mon bloc puis
    je passe au bloc suivant et non pas ecrire 4096B
    comme avant.
    d'ou la nécessité de firstFill et des conditions
    */
    int firstFill = BLOCK_SIZE - f->posSeek % BLOCK_SIZE;
    // save de la data dans le DataBlock
    while (toWrite > 0)
    {
        if (toWrite < BLOCK_SIZE)
        {
            written = toWrite;
        }
        else
        {
            if (toWrite > firstFill)
            {
                written = firstFill;
                firstFill = -1;
            }
            else
            {
                written = BLOCK_SIZE;
            }
        }
        if ((nbWrite = write(fd, (char *)(buffer) + offset, written)) == -1)
        {
            close(fd);
            perror("error write myWrite");
            return -1;
        }
        toWrite -= nbWrite;
        offset += nbWrite;
        if (toWrite > 0)
        {
            iFreeBlock = indexOfFreeBBMP(bbmp);
            bbmp.bmpTab[currentIndex] = iFreeBlock;
            currentIndex = iFreeBlock;
        }
        if (lseek(fd, (currentIndex * BLOCK_SIZE) + DATABLOCK_OFFSET, SEEK_SET) == -1)
        {
            close(fd);
            perror("error seek myWrite");
            return -1;
        }
    }
    // si on ecrit dans un nouveau bloc alors on doit le marquer comme fin
    if (bbmp.bmpTab[currentIndex] == 0)
    {
        bbmp.bmpTab[currentIndex] = USHRT_MAX;
    }
    if (X > f->size)
    {
        f->size = (unsigned int)X;
    }
    f->posSeek = f->posSeek + (unsigned int)nBytes;
    close(fd);
    // Superblock saved
    saveSuperBlock(sb);
    // BitmapBlock saved
    saveBBMP(bbmp);
    // FileBlock saved
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);
    int i = 0;
    int nbActuelFile = sb.totalFile - sb.nbFileDispo;
    while (i < nbActuelFile && strcmp(fileArray[i].nom, f->nom) != 0)
    {
        i++;
    }
    saveFileBlock(*f, i);
    return nBytes;
}

int myRead(File *f, void *buffer, int nBytes)
{
    // Check read permissions
    if ((f->permissions & 0400) == 0) { // Check if read bit is set for owner
        perror("Read permission denied");
        return -1;
    }

    if (nBytes < 0)
    {
        perror("Cannot read negative value");
        return -1;
    }

    // Handle symbolic links
    if (f->linkType == LINK_TYPE_SYMBOLIC)
    {
        // For symbolic links, return the target path
        if (strlen(f->targetPath) == 0)
        {
            perror("Symbolic link has no target path");
            return -1;
        }
        int targetLen = strlen(f->targetPath);
        if (nBytes > targetLen)
        {
            nBytes = targetLen;
        }
        strncpy(buffer, f->targetPath, nBytes);
        // Ensure null termination if we're reading the entire string
        if (nBytes >= targetLen)
        {
            ((char *)buffer)[targetLen] = '\0';
        }
        return nBytes;
    }

    // Handle hard links
    if (f->linkType == LINK_TYPE_HARD)
    {
        // For hard links, we need to read from the target file
        SuperBlock sb;
        loadSuperBlock(&sb);
        File fileArray[NUMBER_OF_BLOCK];
        loadFileBlock(fileArray);

        // Find the target file by its block index
        File *target = NULL;
        int nbActualFiles = sb.totalFile - sb.nbFileDispo;
        for (int i = 0; i < nbActualFiles; i++)
        {
            if (fileArray[i].posInBlockBMP == f->targetFileIndex)
            {
                target = &fileArray[i];
                break;
            }
        }

        if (!target)
        {
            perror("Target file not found for hard link");
            return -1;
        }

        // Check target file read permissions
        if ((target->permissions & 0400) == 0) {
            perror("Read permission denied for target file");
            return -1;
        }

        // Create a temporary file structure for reading
        File tempFile = *target;
        tempFile.posSeek = f->posSeek; // Use the current seek position

        // Read from the target file
        int result = myRead(&tempFile, buffer, nBytes);
        if (result > 0)
        {
            f->posSeek += result; // Update the link's position
        }
        return result;
    }

    // Regular file reading
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    int currentIndex = indexBBMPOfPosSeekLoaded(f, bbmp);
    int toRead = nBytes;

    int fd = open(PARTITION_NAME, O_RDONLY);
    if (fd == -1)
    {
        perror("couldn't open partition myRead");
        return -1;
    }
    // Comme pour read() on lit à partir de la posSeek
    if (lseek(fd, (currentIndex * BLOCK_SIZE) + DATABLOCK_OFFSET + f->posSeek % BLOCK_SIZE, SEEK_SET) == -1)
    {
        close(fd);
        perror("error initial seek myRead");
        return -1;
    }

    int firstRead = BLOCK_SIZE - f->posSeek % BLOCK_SIZE;
    unsigned int readB, offset = 0;
    int end = 0, isFirstRead = 1;
    // On sort si on a finit de lire OU on est arrivé a la fin du fichier
    while (!end)
    {
        // imaginons que on a plus que 200 B dans notre fichier mais qu'on veut lire encore 500
        if (f->size - (f->posSeek + offset) < toRead)
        {
            readB = f->size - (f->posSeek + offset);
        }
        else
        {
            // La premiere fois on doit lire jusquau bout du bloc disque
            //(posSeek : 3200 => lire 896 pour completer le bloc disque)
            if (isFirstRead && firstRead < toRead)
            {
                readB = firstRead;
                isFirstRead = 0;
            }
            else
            {
                readB = toRead;
            }
        }
        if (read(fd, (char *)buffer + offset, readB) == -1)
        {
            close(fd);
            perror("error read myRead");
            return -1;
        }
        offset += readB;
        toRead -= readB;
        if (toRead <= 0 || offset >= f->size)
        {
            end = 1;
        }
        else
        {
            currentIndex = bbmp.bmpTab[currentIndex];
            if (lseek(fd, (currentIndex * BLOCK_SIZE) + DATABLOCK_OFFSET, SEEK_SET) == -1)
            {
                close(fd);
                perror("error seek myRead");
                return -1;
            }
        }
    }
    close(fd);
    f->posSeek += offset;
    return offset;
}
void mySeek(File *f, int offset, int base)
{
    int currentPos = (int)f->posSeek;
    int sizeFile = (int)f->size;
    switch (base)
    {
    case MYSEEK_START:
        if (offset < 0)
        {
            perror("seek negative from start mySeek");
            break;
        }
        if (offset > sizeFile)
        {
            perror("seek outofbound from start mySeek");
            break;
        }
        f->posSeek = offset;
        break;
    case MYSEEK_CUR:
        if ((currentPos + offset) < 0)
        {
            perror("seek outofbound from current mySeek");
            break;
        }

        if ((currentPos + offset) > sizeFile)
        {
            int nbEmpty = (currentPos + offset) - sizeFile;
            f->posSeek = f->size;
            char *emptyNull = (char *)malloc((nbEmpty + 1) * sizeof(char));
            for (int i = 0; i < nbEmpty; i++)
            {
                emptyNull[i] = '\0';
            }
            myWrite(f, emptyNull, nbEmpty);
            break;
        }

        f->posSeek += (unsigned int)offset;
        break;
    case MYSEEK_END:
        if (offset < 0)
        {
            perror("seek negative from end mySeek");
            break;
        }
        if (offset > sizeFile)
        {
            perror("seek outofbound from end mySeek");
            break;
        }
        f->posSeek = sizeFile - offset;
        break;
    default:
        perror("Unknown base mySeek");
        return;
    }
}

int myDelete(char *fileName, int dirID)
{
    SuperBlock sb;
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    Directory dirArray[MAX_DIR_AMOUNT];
    loadDirBlock(dirArray);

    int nbActu = sb.totalFile - sb.nbFileDispo;
    int fileIndex = -1;

    for (int i = 0; i < nbActu; i++)
    {
        if (strcmp(fileArray[i].nom, fileName) == 0 && fileArray[i].parentIndex == dirID)
        {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1)
    {
        perror("File not found");
        return -1;
    }

    File *fileToDelete = &fileArray[fileIndex];

    Directory *parentDir = NULL;
    for (int j = 0; j < MAX_DIR_AMOUNT; j++)
    {
        if (dirArray[j].repoID == fileToDelete->parentIndex)
        {
            parentDir = &dirArray[j];
            break;
        }
    }

    if (!parentDir)
    {
        perror("Parent directory not found");
        return -1;
    }

    // === Handle Block Bitmap Release ===
    int currentIndex = fileToDelete->posInBlockBMP;
    int next = bbmp.bmpTab[currentIndex];

    if (fileToDelete->linkType == LINK_TYPE_SYMBOLIC)
    {
        while (next != USHRT_MAX)
        {
            bbmp.bmpTab[currentIndex] = 0;
            currentIndex = next;
            next = bbmp.bmpTab[currentIndex];
            sb.nbBlockDispo++;
        }
        bbmp.bmpTab[currentIndex] = 0;
        sb.nbBlockDispo++;
    }
    else if (fileToDelete->linkType == LINK_TYPE_HARD)
    {
        int linkCount = 0;
        for (int j = 0; j < nbActu; j++)
        {
            if (fileArray[j].linkType == LINK_TYPE_HARD &&
                fileArray[j].targetFileIndex == fileToDelete->targetFileIndex)
            {
                linkCount++;
            }
        }

        if (linkCount == 1)
        {
            while (next != USHRT_MAX)
            {
                bbmp.bmpTab[currentIndex] = 0;
                currentIndex = next;
                next = bbmp.bmpTab[currentIndex];
                sb.nbBlockDispo++;
            }
            bbmp.bmpTab[currentIndex] = 0;
            sb.nbBlockDispo++;
        }
    }
    else // Regular file
    {
        while (next != USHRT_MAX)
        {
            bbmp.bmpTab[currentIndex] = 0;
            currentIndex = next;
            next = bbmp.bmpTab[currentIndex];
            sb.nbBlockDispo++;
        }
        bbmp.bmpTab[currentIndex] = 0;
        sb.nbBlockDispo++;
    }

    // === Remove file from directory ===
    for (int j = 0; j < parentDir->nbFiles; j++)
    {
        if (strcmp(fileArray[parentDir->files[j]].nom, fileToDelete->nom) == 0)
        {
            // Shift left
            for (int k = j; k < parentDir->nbFiles - 1; k++)
            {
                parentDir->files[k] = parentDir->files[k + 1];
                strcpy(fileArray[parentDir->files[k]].nom, fileArray[parentDir->files[k + 1]].nom);
            }
            parentDir->nbFiles--;
            break;
        }
    }

    // === Remove file from fileArray ===
    for (int j = fileIndex; j < nbActu - 1; j++)
    {
        fileArray[j] = fileArray[j + 1];
    }

    // Clear last file slot
    memset(&fileArray[nbActu - 1], 0, sizeof(File));
    sb.nbFileDispo++;

    // === Save everything ===
    saveSuperBlock(sb);
    for (int j = 0; j < nbActu - 1; j++)
    {
        saveFileBlock(fileArray[j], j);
    }
    saveBBMP(bbmp);

    for (int j = 0; j < MAX_DIR_AMOUNT; j++)
    {
        if (dirArray[j].repoID == parentDir->repoID)
        {
            dirArray[j] = *parentDir; // Explicitly update
            break;
        }
    }
    saveDirBlock(dirArray);

    return 0;
}

unsigned int mySize(File *f)
{
    return f->size;
}

unsigned int myTell(File *f)
{
    return f->posSeek;
}

int myRename(char *oldName, char *newName)
{
    if (strlen(newName) > MAX_FILES_NAME_SIZE)
    {
        perror("New name exceeded size limit myRename");
        return -1;
    }
    SuperBlock sb;
    loadSuperBlock(&sb);
    File array[NUMBER_OF_BLOCK];
    loadFileBlock(array);
    int i = 0;
    int nbActuel = sb.totalFile - sb.nbFileDispo;
    while (i < nbActuel && strcmp(oldName, array[i].nom) != 0)
    {
        i++;
    }
    if (i < nbActuel)
    {
        File tmp = array[i];
        for (int j = 0; j < strlen(newName); j++)
        {
            tmp.nom[j] = newName[j];
        }
        for (int j = strlen(newName); j < MAX_FILES_NAME_SIZE - 1; j++)
        {
            tmp.nom[j] = '\0';
        }
        saveFileBlock(tmp, i);
        return 0;
    }
    perror("File not found myRename");
    return -1;
}

int myCreateRepo(const char *repoName, unsigned short parentID)
{
    // 1. Load the SuperBlock to check directory limits
    SuperBlock sb;
    loadSuperBlock(&sb);
    if (sb.nbBlockDispo == 0)
    {
        printf("No free directory slots left!\n");
        return -1;
    }

    // 2. Load all directories to find a free slot
    Directory dirArray[MAX_DIR_AMOUNT];
    if (loadDirBlock(dirArray) == -1)
    {
        printf("Failed to load directories!\n");
        return -1;
    }

    // 3. Find an empty slot in dirArray
    int freeIndex = -1;
    for (int i = 1; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == 0)
        {
            // repoID=0 means unused
            freeIndex = i;
            break;
        }
    }
    if (freeIndex == -1)
    {
        printf("Directory table full!\n");
        return -1;
    }

    // 4. Verify parent exists (if not root)
    printf("Parent ID: %u\n", parentID);
    Directory *parentDir = NULL; // Default to root
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == parentID)
        {
            parentDir = &dirArray[i];
            printf("Found parent directory: %s\n", parentDir->nomDossier);
            break;
        }
    }
    if (parentDir == NULL)
    {
        printf("Parent directory not found!\n");
        return -1;
    }

    // 5. Initialize the new directory
    Directory newDir;
    strncpy(newDir.nomDossier, repoName, MAX_FILES_NAME_SIZE);
    newDir.repoID = generateUniqueRepoID(); // Implement this (e.g., increment counter)
    newDir.parentID = parentID;
    newDir.nbFiles = 0;
    newDir.nbSubRepos = 0;
    memset(newDir.files, 0, sizeof(newDir.files));
    memset(newDir.subRepos, 0, sizeof(newDir.subRepos));

    if (parentDir->nbSubRepos >= MAX_DIR_AMOUNT)
    {
        printf("Parent directory subfolder limit reached!\n");
        return -1;
    }
    parentDir->subRepos[parentDir->nbSubRepos++] = newDir.repoID;

    // CRITICAL: Update the parent in the dirArray
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == parentID)
        {
            dirArray[i] = *parentDir; // Update the array copy
            break;
        }
    }

    // 7. Save the new directory and update SuperBlock
    dirArray[freeIndex] = newDir;
    if (saveDirBlock(dirArray) == -1)
    {
        printf("Failed to save directory changes!\n");
        return -1;
    }

    sb.nbDirectoryDispo--;
    sb.totalDirectory++;
    if (saveSuperBlock(sb) == -1)
    {
        printf("Warning: Directory created but superblock update failed!\n");
        return -1;
    }

    printf("Created directory '%s' (ID: %u) (parentID : %u)\n", repoName, newDir.repoID, parentDir->repoID);
    return 0;
}

int myDeleteDir(const char *dirname, short parentID)
{
    // Load directory information
    Directory dirArray[MAX_DIR_AMOUNT];
    if (loadDirBlock(dirArray) == -1)
    {
        perror("Failed to load directory block");
        return -1;
    }

    // Find the target directory
    int targetIndex = -1;
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (strcmp(dirArray[i].nomDossier, dirname) == 0 &&
            dirArray[i].parentID == parentID)
        {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex == -1)
    {
        printf("Directory '%s' not found in parent directory ID %d\n", dirname, parentID);
        return -1;
    }

    Directory *targetDir = &dirArray[targetIndex];

    // Check if directory is empty
    if (targetDir->nbFiles > 0 || targetDir->nbSubRepos > 0)
    {
        printf("Directory '%s' is not empty! (%d files, %d subdirectories)\n",
               dirname, targetDir->nbFiles, targetDir->nbSubRepos);
        return -1;
    }

    // Special case: Can't delete root directory
    if (targetDir->repoID == 0)
    {
        printf("Cannot delete root directory!\n");
        return -1;
    }

    // Remove from parent's subRepos list (if not root)
    if (parentID != 0)
    {
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            if (dirArray[i].repoID == parentID)
            {
                // Find and remove from parent's subRepos
                for (int j = 0; j < dirArray[i].nbSubRepos; j++)
                {
                    if (dirArray[i].subRepos[j] == targetDir->repoID)
                    {
                        // Shift remaining entries
                        for (int k = j; k < dirArray[i].nbSubRepos - 1; k++)
                        {
                            dirArray[i].subRepos[k] = dirArray[i].subRepos[k + 1];
                        }
                        dirArray[i].nbSubRepos--;
                        break;
                    }
                }
                break;
            }
        }
    }

    // Clear the directory entry
    memset(targetDir, 0, sizeof(Directory));

    // Update superblock
    SuperBlock sb;
    loadSuperBlock(&sb);
    sb.nbDirectoryDispo++;
    saveSuperBlock(sb);

    // Save directory changes
    if (saveDirBlock(dirArray) == -1)
    {
        perror("Failed to save directory block");
        return -1;
    }

    printf("Directory '%s' deleted successfully\n", dirname);
    return 0;
}
/**
 * Closes a file and updates filesystem metadata
 * @param file Pointer to the file to close
 * @return 0 on success, -1 on failure
 */
int myClose(File *file)
{
    if (file == NULL)
    {
        return -1; // Invalid file pointer
    }

    // Update file metadata if modified (e.g., size changes)
    SuperBlock sb;
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);

    // Find the file in the file block
    int fileIndex = -1;
    for (int i = 0; i < (sb.totalFile - sb.nbFileDispo); i++)
    {
        if (fileArray[i].posInBlockBMP == file->posInBlockBMP &&
            strcmp(fileArray[i].nom, file->nom) == 0)
        {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex != -1)
    {
        // Update file metadata if needed
        fileArray[fileIndex].size = file->size;
        fileArray[fileIndex].posSeek = 0; // Reset seek position on close
        saveFileBlock(fileArray[fileIndex], fileIndex);
    }

    // Free the file memory
    free(file);
    return 0;
}
// Fonction to change file permissions
/**
 * Changes the permissions of a file
 * @param fileName Name of the file
 * @param permissions New permissions (e.g., 0755)
 * @param dirID Directory ID where the file is located
 * @return 0 on success, -1 on failure
 */
int myChmod(char *fileName, unsigned short permissions, short dirID)
{
    // Charger les métadonnées nécessaires
    SuperBlock sb;
    loadSuperBlock(&sb);
    File fileArray[NUMBER_OF_BLOCK];
    loadFileBlock(fileArray);

    // Trouver le fichier
    int fileIndex = -1;
    int nbActualFiles = sb.totalFile - sb.nbFileDispo;
    for (int i = 0; i < nbActualFiles; i++)
    {
        if (strcmp(fileArray[i].nom, fileName) == 0 && fileArray[i].parentIndex == dirID)
        {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1)
    {
        printf("File '%s' not found in directory %d\n", fileName, dirID);
        return -1;
    }

    printf("DEBUG: Changing permissions of %s to %o\n", fileName, permissions);
    fileArray[fileIndex].permissions = permissions;
    if (saveFileBlock(fileArray[fileIndex], fileIndex) == -1) {
        printf("Failed to save file permissions\n");
    return -1;
    }
    printf("DEBUG: Permissions successfully updated on disk\n");

    return 0;
}

int findParentDirID(const char *path, short curIdex)

{
    // Load the directory block
    Directory dirArray[MAX_DIR_AMOUNT];
    if (loadDirBlock(dirArray) == -1)
    {
        perror("Failed to load directory block");
        return -1;
    }

    if (path == NULL || path[0] != '/')
    {
        printf("Invalid path!\n");
        return -1;
    }

    // Split the path into components
    char *pathCopy = strdup(path);
    char *token = strtok(pathCopy, "/");
    int parentID = curIdex; // Start with root directory

    while (token != NULL)
    {
        // Find the directory by name and parent ID
        int found = 0;
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            if (strcmp(dirArray[i].nomDossier, token) == 0 && dirArray[i].parentID == parentID)
            {
                parentID = dirArray[i].repoID;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            free(pathCopy);
            return -1; // Directory not found
        }
        token = strtok(NULL, "/");
    }

    free(pathCopy);
    return parentID;
}

