#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include "../include/process/Fichier.h"
#include "../include/process/Partition.h"
#include "../include/data/TypeSuperBloc.h"
#include "../include/data/TypeDossier.h"
#include "../include/util/constant.h"
#include "../include/util/Tools.h"

char *PARTITION_NAME;
short currentDirectoryID = 0;
void pauseEnter();
void showHelpMessage();
void displayMenu();
void handleCreateFile();
void handleDeleteFile();
void handleRenameFile();
void handleReadFile();
void handleModifyFile();
void handleFormatPartition();
void handleVisualizePartition();
// functions for folder management
void handleCreateFolder();
void handleDeleteFolder();
void handleRenameFolder();
void handleListFolderContents();
void handleCreateSymbolicLink();
void handleCreateHardLink();
void handleReadLink();

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10

void parseCommand(char *command, char **args, int *argc)
{
    char *token = strtok(command, " \n");
    *argc = 0;

    while (token != NULL && *argc < MAX_ARGS)
    {
        args[*argc] = token;
        (*argc)++;
        token = strtok(NULL, " \n");
    }
}

void executeCommand(char **args, int argc)
{
    if (argc == 0)
        return;

    if (strcmp(args[0], "help") == 0)
    {
        printf("Available commands:\n");
        printf("  create file <name> - Create a new file\n");
        printf("  delete file <name> - Delete a file\n");
        printf("  rename file <old> <new> - Rename a file\n");
        printf("  read file <name> - Read file contents\n");
        printf("  modify file <name> - Modify file contents\n");
        printf("  ls - List directory contents\n");
        printf("  mkdir <name> - Create a new directory\n");
        printf("  rmdir <name> - Delete a directory\n");
        printf("  cd <path> - Change directory\n");
        printf("  format - Format the partition\n");
        printf("  symlink <target> <link> - Create symbolic link\n");
        printf("  hardlink <target> <link> - Create hard link\n");
        printf("  readlink <link> - Read symbolic link\n");
        printf("  visualize - Visualize partition\n");
        printf("  exit - Exit the program\n");
    }
    else if (strcmp(args[0], "create") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: create file <name>\n");
            return;
        }
        char *filename = args[2];
        File *tmp = myOpen(filename, currentDirectoryID);
        if (!tmp)
        {
            printf("Failed to create file '%s' in directory '%d'\n", filename, currentDirectoryID);
            return;
        }
        printf("File created successfully. Use 'modify file %s' to add content.\n", filename);
        myClose(tmp);
    }
    else if (strcmp(args[0], "delete") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: delete file <name>\n");
            return;
        }
        if (myDelete(args[2]) == -1)
        {
            printf("Error when deleting file (does not exist or bad match), case sensitive\n");
            return;
        }
        printf("Successfully deleted file: %s\n", args[2]);
    }
    else if (strcmp(args[0], "rename") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: rename file <old> <new>\n");
            return;
        }
        if (myRename(args[2], args[3]) == -1)
        {
            printf("Error when renaming file (does not exist or bad match), case sensitive\n");
            return;
        }
        printf("Successfully renamed file from %s to %s\n", args[2], args[3]);
    }
    else if (strcmp(args[0], "read") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: read file <name>\n");
            return;
        }
        File *tmp = myOpen(args[2], currentDirectoryID);
        if (!tmp)
        {
            printf("Failed to open file '%s'\n", args[2]);
            return;
        }
        char *msg = (char *)malloc((1 + tmp->size) * sizeof(char));
        int totalRead = myRead(tmp, msg, tmp->size);
        if (totalRead == -1)
        {
            free(msg);
            myClose(tmp);
            printf("Error when reading file\n");
            return;
        }
        msg[totalRead] = '\0';
        printf("%s\n", msg);
        free(msg);
        myClose(tmp);
    }
    else if (strcmp(args[0], "modify") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: modify file <name> <content>\n");
            return;
        }
        File *tmp = myOpen(args[2], currentDirectoryID);
        if (!tmp)
        {
            printf("Failed to open file '%s'\n", args[2]);
            return;
        }
        // Combine all remaining arguments as content
        char content[2049] = "";
        for (int i = 3; i < argc; i++)
        {
            strcat(content, args[i]);
            if (i < argc - 1)
                strcat(content, " ");
        }
        if (myWrite(tmp, content, strlen(content)) == -1)
        {
            myClose(tmp);
            printf("Error when writing to file\n");
            return;
        }
        printf("Successfully modified file '%s'\n", args[2]);
        myClose(tmp);
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        handleListFolderContents();
    }
    else if (strcmp(args[0], "mkdir") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: mkdir <name>\n");
            return;
        }
        if (myCreateRepo(args[1], currentDirectoryID) == -1)
        {
            printf("Failed to create folder '%s'\n", args[1]);
        }
        else
        {
            printf("Folder '%s' created successfully\n", args[1]);
        }
    }
    else if (strcmp(args[0], "rmdir") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: rmdir <name>\n");
            return;
        }
        if (myDeleteDir(args[1]) == -1)
        {
            printf("Failed to delete folder '%s' (might not be empty or doesn't exist)\n", args[1]);
        }
        else
        {
            printf("Folder '%s' deleted successfully\n", args[1]);
        }
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: cd <path>\n");
            return;
        }
        Directory dirArray[MAX_DIR_AMOUNT];
        if (loadDirBlock(dirArray) == -1)
        {
            printf("Failed to load directory information\n");
            return;
        }

        // Handle special cases
        if (strcmp(args[1], "..") == 0)
        {
            if (currentDirectoryID == 0)
            {
                printf("Already at root directory\n");
                return;
            }
            for (int i = 0; i < MAX_DIR_AMOUNT; i++)
            {
                if (dirArray[i].repoID == currentDirectoryID)
                {
                    currentDirectoryID = dirArray[i].parentID;
                    printf("Changed to parent directory\n");
                    return;
                }
            }
        }
        else if (strcmp(args[1], "/") == 0)
        {
            currentDirectoryID = 0;
            printf("Changed to root directory\n");
            return;
        }

        // Handle subdirectory navigation
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            if (dirArray[i].parentID == currentDirectoryID &&
                strcmp(dirArray[i].nomDossier, args[1]) == 0)
            {
                currentDirectoryID = dirArray[i].repoID;
                printf("Changed to directory: %s\n", args[1]);
                return;
            }
        }

        printf("Directory '%s' not found\n", args[1]);
    }
    else if (strcmp(args[0], "format") == 0)
    {
        if (argc > 1 && (strcmp(args[1], "-y") == 0 || strcmp(args[1], "--yes") == 0))
        {
            if (myFormat(PARTITION_NAME) == -1)
            {
                printf("Something went wrong when formatting, perhaps we didn't recognized our proprietary label, please delete manually.\n");
                exit(1);
            }
            printf("Successfully formatted partition.\n");
        }
        else
        {
            printf("Warning: This will erase all files in %s\n", PARTITION_NAME);
            printf("Use 'format -y' or 'format --yes' to confirm\n");
        }
    }
    else if (strcmp(args[0], "symlink") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: symlink <target> <link>\n");
            return;
        }
        if (myCreateSymbolicLink(args[2], args[1], currentDirectoryID) == 0)
        {
            printf("Symbolic link created successfully.\n");
        }
        else
        {
            printf("Failed to create symbolic link.\n");
        }
    }
    else if (strcmp(args[0], "hardlink") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: hardlink <target> <link>\n");
            return;
        }
        if (myCreateHardLink(args[2], args[1], currentDirectoryID) == 0)
        {
            printf("Hard link created successfully.\n");
        }
        else
        {
            printf("Failed to create hard link.\n");
        }
    }
    else if (strcmp(args[0], "readlink") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: readlink <link>\n");
            return;
        }
        char *targetPath = myReadLink(args[1], currentDirectoryID);
        if (targetPath != NULL)
        {
            printf("Link points to: %s\n", targetPath);
            free(targetPath);
        }
        else
        {
            printf("Failed to read link.\n");
        }
    }
    else if (strcmp(args[0], "visualize") == 0)
    {
        handleVisualizePartition();
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        printf("Unknown command. Type 'help' for available commands.\n");
    }
}

// Add this function before main
void buildCurrentPath(char *path, Directory *dirArray)
{
    if (currentDirectoryID == 0)
    {
        strcpy(path, "/");
        return;
    }

    // Find the current directory
    Directory *currentDir = NULL;
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == currentDirectoryID)
        {
            currentDir = &dirArray[i];
            break;
        }
    }

    if (!currentDir)
    {
        strcpy(path, "/");
        return;
    }

    // Build path from root to current
    char tempPath[MAX_FILES_NAME_SIZE * 10] = ""; // Increased size for longer paths
    strcpy(tempPath, currentDir->nomDossier);

    // Add parent directories
    short parentID = currentDir->parentID;
    while (parentID != 0)
    {
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            if (dirArray[i].repoID == parentID)
            {
                char temp[MAX_FILES_NAME_SIZE * 10] = "";
                strcpy(temp, dirArray[i].nomDossier);
                strcat(temp, "/");
                strcat(temp, tempPath);
                strcpy(tempPath, temp);
                parentID = dirArray[i].parentID;
                break;
            }
        }
    }

    // Add root prefix
    strcpy(path, "/");
    strcat(path, tempPath);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: fileManager [partitionName]\n");
        printf("Try 'fileManager -h' for more information.\n");
        return 0;
    }
    if (strcmp(argv[1], "-h") == 0)
    {
        showHelpMessage();
        return 0;
    }

    PARTITION_NAME = argv[1];
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int argCount;
    char currentPath[MAX_FILES_NAME_SIZE * 10] = ""; // Increased size for longer paths
    Directory dirArray[MAX_DIR_AMOUNT];

    printf("File Manager CLI (Working on: %s)\n", PARTITION_NAME);
    printf("Type 'help' for available commands\n\n");

    while (1)
    {
        // Load directory information and build current path
        if (loadDirBlock(dirArray) != -1)
        {
            buildCurrentPath(currentPath, dirArray);
        }
        else
        {
            strcpy(currentPath, "/");
        }

        printf("filemanager %s%s> ", PARTITION_NAME, currentPath);
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL)
        {
            break;
        }

        // Remove trailing newline
        command[strcspn(command, "\n")] = 0;

        // Skip empty commands
        if (strlen(command) == 0)
        {
            continue;
        }

        parseCommand(command, args, &argCount);
        executeCommand(args, argCount);
    }

    return 0;
}

void showHelpMessage()
{
    printf("Usage: fileManager [partitionName]\n");
    printf("Description:\n");
    printf("  This program is a simple file manager simulation.\n");
    printf("  It allows you to perform various operations on a virtual disk partition.\n\n");
    printf("Example:\n");
    printf("  fileManager maPartition.bin\n");
    printf("    Launch the program and open the specified partition to work on.\n\n");
}

void displayMenu()
{
    int choice;
    do
    {
        printf("\nMenu: (working on : %s)\n", PARTITION_NAME);
        printf("1. Create File\n");
        printf("2. Delete File\n");
        printf("3. Rename File\n");
        printf("4. Read File\n");
        printf("5. Modify File\n");
        printf("6. List Directory Contents\n");
        printf("7. Create Directory\n");
        printf("8. Delete Directory\n");
        printf("9. Change Directory\n");
        printf("10. Format Partition\n");
        printf("11. Create Symbolic Link\n");
        printf("12. Create Hard Link\n");
        printf("13. Read Symbolic Link\n");
        printf("14. Visualize Partition\n");
        printf("15. Exit\n");
        printf("Choice: ");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            handleCreateFile();
            break;
        case 2:
            handleDeleteFile();
            break;
        case 3:
            handleRenameFile();
            break;
        case 4:
            handleReadFile();
            break;
        case 5:
            handleModifyFile();
            break;
        case 6:
            handleListFolderContents();
            break;
        case 7:
            handleCreateFolder();
            break;
        case 8:
            handleDeleteFolder();
            break;
        case 9:
            handleChangeDirectory();
            break;
        case 10:
            handleFormatPartition();
            break;
        case 11:
            handleCreateSymbolicLink();
            break;
        case 12:
            handleCreateHardLink();
            break;
        case 13:
            handleReadLink();
            break;
        case 14:
            handleVisualizePartition();
            break;
        case 15:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 15);
}

void handleCreateFile()
{
    char buf[128] = "";
    char bigBuffer[2049] = "";
    File *tmp;
    printf("File name : \n");
    scanf(" %s", buf);

    // Create file with validation
    tmp = myOpen(buf, currentDirectoryID);
    if (!tmp)
    {
        printf("Failed to create file '%s' in directory '%d'\n",
               buf, currentDirectoryID);
        return;
    }

    printf("Content (2048 char max, end with '.' on new line):\n");
    while (1)
    {
        // Clear buffer before each read
        memset(bigBuffer, 0, sizeof(bigBuffer));

        if (scanf(" %2048[^\n]", bigBuffer) != 1)
        { // Safer input
            printf("Error reading input\n");
            myClose(tmp);
            return;
        }

        if (bigBuffer[0] == '.')
        {
            break;
        }

        if (myWrite(tmp, bigBuffer, strlen(bigBuffer)) == -1)
        {
            printf("Error writing to file (possibly out of space)\n");
            myClose(tmp);
            return;
        }
    }

    printf("Successfully created '%s' in %d\n", tmp->nom, currentDirectoryID);
    myClose(tmp);
}

void handleDeleteFile()
{
    char buf[128] = "";
    printf("Specify which file to delete : ");
    scanf(" %[^\n]s", buf);
    if (strlen(buf) > MAX_FILES_NAME_SIZE)
    {
        printf("Filename cannot exceed %u char", MAX_FILES_NAME_SIZE);
        return;
    }
    if (myDelete(buf) == -1)
    {
        printf("Error when deleting file (does not exist or bad match), case sensitive\n");
        return;
    }
    printf("Successfully deleted file : %s", buf);
}
void handleRenameFile()
{
    char buf[128] = "";
    char rename[65] = "";
    printf("Specify which file to rename: \n");
    scanf(" %[^\n]s", buf);
    printf("Renaming to : \n");
    scanf(" %[^\n]s", rename);
    if (strlen(buf) > MAX_FILES_NAME_SIZE || strlen(rename) > MAX_FILES_NAME_SIZE)
    {
        printf("Filename cannot exceed %u char", MAX_FILES_NAME_SIZE);
        return;
    }
    if (myRename(buf, rename) == -1)
    {
        printf("Error when renaming file (does not exist or bad match), case sensitive\n");
        return;
    }
    printf("Successfully renamed file from : %s to : %s ", buf, rename);
}

void handleReadFile()
{
    char buf[128] = "";
    int nbToRead = 0;
    int offset = 0;
    int base = 0;
    int totalRead;
    File *tmp;
    printf("File name : ");
    scanf(" %s", buf);
    if ((tmp = myOpen(buf, currentDirectoryID)) == NULL)
    {
        printf("Something went wrong when opening file.\n");
        exit(1);
    }
    printf("\nHow much char (-1 to show all) : ");
    scanf(" %d", &nbToRead);
    if (nbToRead != -1)
    {
        printf("\nWhich base : \n");
        printf("1. Start\n");
        printf("2. Specific\n");
        printf("3. End\n");
        scanf(" %d", &base);
        if (base > 3 || base < 1)
        {
            free(tmp);
            printf("Error when choosing Base");
            return;
        }

        printf("Offset  : \n");
        scanf(" %d", &offset);
        char *bufferMsg = (char *)malloc((nbToRead + 1) * sizeof(nbToRead));
        mySeek(tmp, offset, base - 1);

        if ((totalRead = myRead(tmp, bufferMsg, nbToRead)) == -1)
        {
            free(tmp);
            free(bufferMsg);
            printf("Error when reading file");
            return;
        }
        bufferMsg[totalRead] = '\0';
        printf("%s", bufferMsg);
    }
    else
    {
        char *msg = (char *)malloc((1 + tmp->size) * sizeof(char));
        if ((totalRead = myRead(tmp, msg, tmp->size)) == -1)
        {
            free(tmp);
            free(msg);
            printf("Error when reading the whole file\n");
            return;
        }
        msg[totalRead] = '\0';
        printf("%s\n", msg);
        free(msg);
    }
    free(tmp);
}

void handleModifyFile()
{
    char buf[128] = "";
    char bufMsg[2049] = "";

    int offset = 0;
    int base = 0;
    File *tmp;
    printf("File name : ");
    scanf(" %s", buf);
    if ((tmp = myOpen(buf, currentDirectoryID)) == NULL)
    {
        printf("Something went wrong when opening file.\n");
        exit(1);
    }
    printf("\nWhich base : \n");
    printf("1. Start\n");
    printf("2. Specific\n");
    printf("3. End\n");
    scanf(" %d", &base);
    if (base > 3 || base < 1)
    {
        free(tmp);
        printf("Error when choosing Base");
        return;
    }

    printf("Offset  : \n");
    scanf(" %d", &offset);

    mySeek(tmp, offset, base - 1);

    printf("Content (2048 char max per input) (terminate writing with a dot '.' on a new line): \n");
    while (bufMsg[0] != '.')
    {
        scanf(" %[^\n]s", bufMsg);
        bufMsg[2048] = '\0';
        if (myWrite(tmp, bufMsg, strlen(bufMsg)) == -1)
        {
            free(tmp);
            printf("Error when writing to file");
            return;
        }
    }
    free(tmp);
}

void handleFormatPartition()
{
    char accept = 'a';
    printf("This action will erase all file in %s, are you sure ? (Takes 30-50 seconds) (Y/N) \n", PARTITION_NAME);
    scanf(" %c", &accept);
    if (accept == 'Y' || accept == 'y')
    {
        if (myFormat(PARTITION_NAME) == -1)
        {
            printf("Something went wrong when formatting, perhaps we didn't recognized our proprietary label, please delete manually.\n");
            exit(1);
        }
        printf("Successfully formated partition.\n");
    }
}
void handleVisualizePartition()
{
    SuperBlock sb;
    loadSuperBlock(&sb);
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    File array[NUMBER_OF_BLOCK];
    loadFileBlock(array);
    printSB(sb);
    printBBMP(bbmp);
    printf("Data disk usage : %.3f%%\n", (float)(sb.totalBlock - sb.nbBlockDispo) / (float)sb.totalBlock);
    int nbFile = sb.totalFile - sb.nbFileDispo;
    int cpt = 0;
    for (int i = 0; i < nbFile; i++)
    {
        cpt = array[i].posInBlockBMP;
        printf("\tFile : %s\n", array[i].nom);
        printf("\t\tIndex blocks : ");
        while (cpt != USHRT_MAX)
        {
            printf("%u ", cpt);
            cpt = bbmp.bmpTab[cpt];
            if (cpt != USHRT_MAX)
            {
                printf("-> ");
            }
        }
        printf("\n\t\tSize : %uB\n", array[i].size);
    }
}

void handleCreateFolder()
{
    char folderName[MAX_FILES_NAME_SIZE];
    printf("Enter folder name: ");
    scanf(" %[^\n]", folderName);

    if (myCreateRepo(folderName, currentDirectoryID) == -1)
    {
        printf("Failed to create folder '%s'\n", folderName);
    }
    else
    {
        printf("Folder '%s' created successfully\n", folderName);
    }
}

void handleListFolderContents()
{
    Directory dirArray[MAX_DIR_AMOUNT];
    if (loadDirBlock(dirArray) == -1)
    {
        printf("Failed to load directory information\n");
        return;
    }

    printf("\n=== Existing Folders ===\n");

    // Count the number of folders
    int folderCount = 0;
    for (int i = 1; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].parentID == currentDirectoryID && strcmp(dirArray[i].nomDossier, "") != 0)
        {
            // Print folder name
            printf("📁 %s/\n", dirArray[i].nomDossier);
            folderCount++;
        }
    }

    if (folderCount == 0)
    {
        printf("No folders found\n");
    }
    printf("\n=== Existing Files ===\n");
    File fileArray[NUMBER_OF_BLOCK];

    if (loadFileBlock(fileArray) == -1)
    {
        printf("Failed to load files information\n");
        return;
    }

    int fileCount = 0;
    for (int i = 0; i < MAX_ENTRIES_DIR; i++)
    {
        if (fileArray[i].parentIndex == currentDirectoryID && strcmp(fileArray[i].nom, "") != 0)
        {
            printf("📄 %s\n", fileArray[i].nom);
            fileCount++;
        }
    }

    if (fileCount == 0)
    {
        printf("No files found\n");
    }
}

void handleDeleteFolder()
{
    char folderName[MAX_FILES_NAME_SIZE];
    printf("Enter folder name to delete: ");
    scanf(" %[^\n]", folderName);

    if (myDeleteDir(folderName) == -1)
    {
        printf("Failed to delete folder '%s' (might not be empty or doesn't exist)\n", folderName);
    }
    else
    {
        printf("Folder '%s' deleted successfully\n", folderName);
    }
}

/**
 * Changes the current working directory
 */
void handleChangeDirectory()
{
    Directory dirArray[MAX_DIR_AMOUNT];
    if (loadDirBlock(dirArray) == -1)
    {
        printf("Failed to load directory information\n");
        return;
    }

    char targetDir[MAX_FILES_NAME_SIZE];
    printf("\nCurrent directory: %s\n",
           currentDirectoryID == 0 ? "/" : dirArray[findDirIndex(currentDirectoryID, dirArray)].nomDossier);
    printf("Enter directory name (or '..' for parent): ");
    scanf(" %[^\n]", targetDir);

    // Handle parent directory navigation
    if (strcmp(targetDir, "..") == 0)
    {
        if (currentDirectoryID == 0)
        {
            printf("Already at root directory\n");
        }
        else
        {
            // Find and switch to parent
            for (int i = 0; i < MAX_DIR_AMOUNT; i++)
            {
                if (dirArray[i].repoID == currentDirectoryID)
                {
                    currentDirectoryID = dirArray[i].parentID;
                    printf("Changed to parent directory\n");
                    break;
                }
            }
        }
        return;
    }

    // Handle subdirectory navigation
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].parentID == currentDirectoryID &&
            strcmp(dirArray[i].nomDossier, targetDir) == 0)
        {
            currentDirectoryID = dirArray[i].repoID;
            printf("Changed to directory: %s\n", targetDir);
            return;
        }
    }

    printf("Directory '%s' not found\n", targetDir);
}

void handleCreateSymbolicLink()
{
    char linkName[MAX_FILES_NAME_SIZE];
    char targetPath[MAX_FILES_NAME_SIZE];

    printf("Enter the name for the symbolic link: ");
    scanf("%s", linkName);
    printf("Enter the target path: ");
    scanf("%s", targetPath);

    if (myCreateSymbolicLink(linkName, targetPath, currentDirectoryID) == 0)
    {
        printf("Symbolic link created successfully.\n");
    }
    else
    {
        printf("Failed to create symbolic link.\n");
    }
}

void handleCreateHardLink()
{
    char linkName[MAX_FILES_NAME_SIZE];
    char targetFile[MAX_FILES_NAME_SIZE];

    printf("Enter the name for the hard link: ");
    scanf("%s", linkName);
    printf("Enter the target file name: ");
    scanf("%s", targetFile);

    if (myCreateHardLink(linkName, targetFile, currentDirectoryID) == 0)
    {
        printf("Hard link created successfully.\n");
    }
    else
    {
        printf("Failed to create hard link.\n");
    }
}

void handleReadLink()
{
    char linkName[MAX_FILES_NAME_SIZE];

    printf("Enter the name of the symbolic link to read: ");
    scanf("%s", linkName);

    char *targetPath = myReadLink(linkName, currentDirectoryID);
    if (targetPath != NULL)
    {
        printf("Link points to: %s\n", targetPath);
        free(targetPath);
    }
    else
    {
        printf("Failed to read link.\n");
    }
}