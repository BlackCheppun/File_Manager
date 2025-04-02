#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../../include/util/constant.h"
#include "../../include/data/TypeFichier.h"
#include "../../include/data/TypeDossier.h"
#include "../../include/process/Partition.h"

int myFormat(char *nomPartition)
{
    int fd = open(nomPartition, O_CREAT | O_RDWR, 0644);

    if (fd == -1)
    {
        perror("probleme lors de la creation partition");
        return -1;
    }

    // Vérification si contient ALS ou si new file alors on autorise le formatage
    off_t fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1)
    {
        perror("erreur lors de la recherche de la taille du fichier");
        close(fd);
        return -1;
    }
    // si different de 0 alors on check si il y a le tag 'ALS'
    if (fileSize > 0)
    {
        // se positionner sur la bonne zone pour lire
        if (lseek(fd, 14, SEEK_SET) == -1)
        {
            perror("erreur seek myFormat");
            close(fd);
            return -1;
        }
        char FSproperties[4] = "";
        if (read(fd, FSproperties, 4) == -1)
        {
            perror("erreur read properties myFormat");
            close(fd);
            return -1;
        }
        if (strcmp(FSproperties, "ALS") != 0)
        {
            perror("erreur le fichier n'appartient pas a ALS");
            close(fd);
            return -1;
        }
    }

    off_t newPosition = lseek(fd, PARTITION_SIZE - 1, SEEK_SET);

    if (newPosition == -1)
    {
        perror("erreur seek partition");
        close(fd);
        return -1;
    }

    char nullChar = '\0';
    ssize_t bytesWritten = write(fd, &nullChar, sizeof(char));

    if (bytesWritten == -1)
    {
        perror("erreur allocation memoire partition");
        close(fd);
        return -1;
    }

    int nbWrite;
    // Superblock information writing
    lseek(fd, 0, SEEK_SET);
    // unsigned short partitionInfo[7] = {NUMBER_OF_BLOCK, BLOCK_SIZE, NUMBER_OF_BLOCK, NUMBER_OF_BLOCK, NUMBER_OF_BLOCK, MAX_DIR_AMOUNT, MAX_DIR_AMOUNT};
    unsigned short partitionInfo[9] = {
        NUMBER_OF_BLOCK,    // Total blocks
        BLOCK_SIZE,         // Block size
        NUMBER_OF_BLOCK,    // Free blocks (initially all free)
        NUMBER_OF_BLOCK,    // (Unused? Adjust if needed)
        NUMBER_OF_BLOCK,    // (Unused? Adjust if needed)
        MAX_DIR_AMOUNT,     // Total directories
        1,                  // Used directories (root counts as 1)
        MAX_DIR_AMOUNT - 1, // Free directories
        0                   // rootRepoID (0 is root)
    };
    nbWrite = write(fd, partitionInfo, sizeof(partitionInfo));
    char *fs_proprietary = "ALS";
    nbWrite = write(fd, fs_proprietary, sizeof(fs_proprietary));

    // BlockBitmap writing
    unsigned short zeroVal = 0;
    for (int i = 0; i < BLOCK_BITMAP_SIZE; i++)
    {
        nbWrite = write(fd, &zeroVal, sizeof(unsigned short));
        if (nbWrite <= 0)
        {
            perror("erreur lors de l'init du block bitmap");
            close(fd);
            return -1;
        }
    }

    // File Block writing
    for (int i = 0; i < FILE_BLOCK_SIZE; i++)
    {
        nbWrite = write(fd, &nullChar, 1);
        if (nbWrite <= 0)
        {
            perror("erreur lors de l'init du File block");
            close(fd);
            return -1;
        }
    }

    Directory rootDir;
    strcpy(rootDir.nomDossier, "root");
    rootDir.repoID = 0;
    rootDir.parentID = 0;
    rootDir.nbFiles = 0;
    rootDir.nbSubRepos = 0;

    lseek(fd, DIRBLOCK_OFFSET, SEEK_SET);
    nbWrite = write(fd, &rootDir, sizeof(Directory));
    if (nbWrite <= 0)
    {
        perror("erreur lors de l'init du Dir block");
        close(fd);
        return -1;
    }

    // Data Block writing
    char tab[BLOCK_SIZE] = {0};
    for (int i = 0; i < NUMBER_OF_BLOCK; i++)
    {
        nbWrite = write(fd, tab, sizeof(tab));
        if (nbWrite <= 0)
        {
            perror("erreur lors de l'init du Dir block");
            close(fd);
            return -1;
        }
    }

    close(fd);
    return 0;
}