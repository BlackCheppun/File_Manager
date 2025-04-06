/**
 * @file Partition.c
 * @brief Implémentation des fonctions de gestion de la partition
 * @author Massin KHELOUFI (30%) Ilyes AGHOUILES (40%) Imane ABDELBASSIR (30%)
 * 
 * Ce fichier contient l'implémentation des fonctions permettant de gérer
 * la partition virtuelle du système de fichiers, notamment :
 * - Le formatage de la partition
 * - L'initialisation des structures de données (superbloc, bitmap, etc.)
 * - La création du répertoire racine
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <fcntl.h>
 #include <unistd.h>
 #include <string.h>
 #include "../../include/util/constant.h"
 #include "../../include/data/TypeFichier.h"
 #include "../../include/data/TypeDossier.h"
 #include "../../include/process/Partition.h"
 
 /**
  * @brief Formate une partition avec le système de fichiers ALS
  * 
  * Cette fonction crée et initialise une nouvelle partition avec le système de fichiers ALS.
  * Elle effectue les opérations suivantes :
  * 1. Crée/ouvre le fichier de partition
  * 2. Vérifie si le fichier existe déjà et s'il appartient au système ALS
  * 3. Alloue l'espace nécessaire pour la partition
  * 4. Initialise les différentes zones :
  *    - Superbloc (informations générales)
  *    - Bitmap des blocs (gestion de l'espace)
  *    - Table des fichiers
  *    - Table des répertoires (avec création du répertoire racine)
  *    - Zone de données
  *
  * @param nomPartition Nom du fichier représentant la partition
  * @return 0 en cas de succès, -1 en cas d'erreur
  */
 int myFormat(char *nomPartition)
 {
     int fd = open(nomPartition, O_CREAT | O_RDWR | O_TRUNC, 0644);
 
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
         if (lseek(fd, 18, SEEK_SET) == -1)
         {
             perror("erreur seek myFormat");
             close(fd);
             return -1;
         }
 
         char FSproperties[3] = {0};          // Initialise tout à 0
         if (read(fd, FSproperties, 3) == -1) // Lire uniquement 3 caractères
         {
             perror("erreur read properties myFormat");
             close(fd);
             return -1;
         }
 
         if (strncmp(FSproperties, "ALS", 4) != 0)
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
     nbWrite = write(fd, fs_proprietary, strlen(fs_proprietary)); // Écrit 3 bytes et pas un pointeur
 
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
 
     Directory dirArray[MAX_DIR_AMOUNT] = {0};
 
     Directory *rootDir = malloc(sizeof(Directory));
     if (rootDir == NULL)
     {
         perror("erreur lors de l'init du Dir block");
         close(fd);
         return -1;
     }
     strcpy(rootDir->nomDossier, "root");
     rootDir->repoID = 0;
     rootDir->parentID = 0;
     rootDir->nbFiles = 0;
     rootDir->nbSubRepos = 0;
 
     dirArray[0] = *rootDir;
 
     lseek(fd, DIRBLOCK_OFFSET, SEEK_SET);
     nbWrite = write(fd, dirArray, sizeof(dirArray));
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