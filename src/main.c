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
char currentPath[MAX_FILES_NAME_SIZE * 10] = "";
void showHelpMessage();
void handleCreateFile();
void handleRenameFile();
void handleReadFile();
void handleModifyFile();
void handleFormatPartition();
void handleVisualizePartition();
// functions for folder management
void handleCreateFolder();
void handleDeleteFolder();
void handleRenameFolder();
void handleListFolderContents(const char *path);
// functions for link management
void handleCreateSymbolicLink();
void handleCreateHardLink();
void handleReadLink();
void handleChangeDirectory();

// function for permission
void handleChmod();

// functions for backup and restore
void handleBackupPartition();
void handleRestorePartition(char *backup_name);

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
        printf("  create file <n> - Create a new file\n");
        printf("  delete file <n> - Delete a file\n");
        printf("  rename file <old> <new> - Rename a file\n");
        printf("  read file <n> - Read file contents\n");
        printf("  modify file <n> - Modify file contents\n");
        printf("  ls - List directory contents\n");
        printf("  mkdir <n> - Create a new directory\n");
        printf("  rmdir <n> - Delete a directory\n");
        printf("  cd <path> - Change directory\n");
        printf("  format - Format the partition\n");
        printf("  symlink <target> <link> - Create symbolic link\n");
        printf("  hardlink <target> <link> - Create hard link\n");
        printf("  readlink <link> - Read symbolic link\n");
        printf("  visualize - Visualize partition\n");
        printf("  chmod <file> <permissions> - Change file permissions\n");
        printf("  backup - Backup current partition\n");
        printf("  restore <backup_name> - Restore from backup\n");
        printf("  exit - Exit the program\n");
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        printf("Current path: %s\n", currentPath);
    }
    else if (strcmp(args[0], "create") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: create file <name>\n");
            return;
        }

        const char *fullpath = args[2];
        char *pathCopy = strdup(fullpath);
        if (!pathCopy)
        {
            perror("memorry allocation error");
            return;
        }

        // extract the filename
        char *lashSlash = strrchr(pathCopy, '/');
        char *filename = NULL;
        int parentid;
        if (lashSlash)
        {
            *lashSlash = '\0';
            filename = strdup(lashSlash + 1);
            if (strlen(pathCopy) == 0)
            {
                // if the path is empty, set it to the current directory
                parentid = currentDirectoryID;
            }
            else
            {
                // find the parent directory ID
                parentid = findParentDirID(pathCopy, currentDirectoryID);
                if (parentid == -1)
                {
                    printf("Error: Parent directory not found\n");
                    free(pathCopy);
                    return;
                }
            }
        }
        else
        {
            filename = strdup(pathCopy);
            parentid = currentDirectoryID;
        }

        free(pathCopy);

        if (parentid == -1)
        {
            printf("Invalid path. Could not find parent folder.\n");
            return;
        }

        File *tmp = myOpen(filename, parentid);
        if (!tmp)
        {
            printf("Failed to create file '%s' in directory '%d'\n", filename, parentid);
            return;
        }
        printf("File created successfully. Use 'modify file %s' to add content.\n", filename);
        myClose(tmp);
    }
    else if (strcmp(args[0], "delete") == 0 && strcmp(args[1], "file") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: delete file <path/to/filename or filename>\n");
            return;
        }

        char *fullPath = args[2];
        char pathCopy[strlen(fullPath) + 1];
        strcpy(pathCopy, fullPath);

        char *filename = strrchr(pathCopy, '/');

        int parentID;
        if (filename == NULL)
        {
            // No slash: filename in current directory
            filename = fullPath;
            parentID = currentDirectoryID;
        }
        else
        {
            // There is a path: separate parent path and filename
            *filename = '\0'; // Cut pathCopy to get parent path
            filename++;       // Move to filename part
            parentID = findParentDirID(pathCopy, currentDirectoryID);

            if (parentID == -1)
            {
                printf("Invalid path. Could not find parent directory.\n");
                return;
            }
        }

        if (myDelete(filename, parentID) == -1)
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
            printf("Usage: read file <path>\n");
            return;
        }

        // Extract directory path and filename
        char *path = strdup(args[2]);
        char *filename = strrchr(path, '/');

        short parentDirID;
        if (filename == NULL)
        {
            // No path specified, use current directory
            filename = path;
            parentDirID = currentDirectoryID;
        }
        else
        {
            // Split path and filename
            *filename = '\0'; // Terminate the path portion
            filename++;       // Move to filename

            if (strlen(path) == 0)
            {
                // Path was just "/", meaning root
                parentDirID = 0;
            }
            else
            {
                parentDirID = findParentDirID(path, currentDirectoryID);
                if (parentDirID == -1)
                {
                    printf("Path '%s' not found\n", path);
                    free(path);
                    return;
                }
            }
        }

        File *tmp = myOpen(filename, parentDirID);
        //free(path);

        if (!tmp)
        {
            printf("Failed to open file '%s'\n", filename);
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
        if (argc < 4)
        {
            printf("Usage: modify file <path> <content>\n");
            return;
        }

        // Extract directory path and filename
        char *path = strdup(args[2]);
        char *filename = strrchr(path, '/');

        short parentDirID;
        if (filename == NULL)
        {
            // No path specified, use current directory
            filename = path;
            parentDirID = currentDirectoryID;
        }
        else
        {
            // Split path and filename
            *filename = '\0'; // Terminate the path portion
            filename++;       // Move to filename

            if (strlen(path) == 0)
            {
                // Path was just "/", meaning root
                parentDirID = 0;
            }
            else
            {
                parentDirID = findParentDirID(path, currentDirectoryID);
                if (parentDirID == -1)
                {
                    printf("Path '%s' not found\n", path);
                    free(path);
                    return;
                }
            }
        }

        File *tmp = myOpen(filename, parentDirID);
        //free(path);

        if (!tmp)
        {
            printf("Failed to open file '%s'\n", filename);
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

        printf("Successfully modified file '%s'\n", filename);
        myClose(tmp);
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        handleListFolderContents(argc > 1 ? args[1] : NULL);
    }
    else if (strcmp(args[0], "mkdir") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: mkdir <path>\n");
            return;
        }

        // Extract directory path and new folder name
        char *path = strdup(args[1]);
        char *dirname = strrchr(path, '/');

        short parentDirID;
        if (dirname == NULL)
        {
            // No path specified, use current directory
            dirname = path;
            parentDirID = currentDirectoryID;
        }
        else
        {
            // Split path and directory name
            *dirname = '\0'; // Terminate the path portion
            dirname++;       // Move to directory name

            if (strlen(path) == 0)
            {
                // Path was just "/", meaning root
                parentDirID = 0;
            }
            else
            {
                parentDirID = findParentDirID(path, currentDirectoryID);
                if (parentDirID == -1)
                {
                    printf("Parent path '%s' not found\n", path);
                    free(path);
                    return;
                }
            }
        }

        if (myCreateRepo(dirname, parentDirID) == -1)
        {
            printf("Failed to create folder '%s'\n", dirname);
        }
        else
        {
            printf("Folder '%s' created successfully in %s\n",
                   dirname,
                   (parentDirID == currentDirectoryID) ? "current directory" : "specified path");
        }

        free(path);
    }
    else if (strcmp(args[0], "rmdir") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: rmdir <absolute_path>\n");
            return;
        }

        // Only accept absolute paths starting with /
        if (args[1][0] != '/')
        {
            printf("Error: Only absolute paths are supported (e.g., /a/b/c)\n");
            return;
        }

        char *path = strdup(args[1]);
        if (!path)
        {
            printf("Memory allocation error\n");
            return;
        }

        // Find the last component
        char *last_slash = strrchr(path, '/');
        if (!last_slash || strlen(last_slash) <= 1)
        {
            printf("Invalid path format - must be like /a/b/c\n");
            free(path);
            return;
        }

        // Split into parent path and directory name
        *last_slash = '\0';             // Terminate parent path
        char *dirname = last_slash + 1; // Directory to delete

        // Handle root parent case (/dirname)
        short parentDirID = 0; // Start with root
        if (strlen(path) > 0)
        { // If path isn't just "/"
            parentDirID = findParentDirID(path, 0);
            if (parentDirID == -1)
            {
                printf("Parent path '%s' does not exist\n", path);
                free(path);
                return;
            }
        }

        // Check if directory exists
        Directory dirArray[MAX_DIR_AMOUNT];
        if (loadDirBlock(dirArray) == -1)
        {
            printf("Failed to load directory information\n");
            free(path);
            return;
        }

        int dir_found = 0;
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            if (strcmp(dirArray[i].nomDossier, dirname) == 0 &&
                dirArray[i].parentID == parentDirID)
            {
                dir_found = 1;
                break;
            }
        }

        if (!dir_found)
        {
            printf("Directory '%s' not found in '%s'\n", dirname, path);
            free(path);
            return;
        }

        // Delete the directory
        int result = myDeleteDir(dirname, parentDirID);
        //free(path);

        if (result == -1)
        {
            printf("Failed to delete '%s' (directory not empty or system error)\n", dirname);
        }
        else
        {
            printf("Successfully deleted '%s'\n", dirname);
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

        // Handle special cases first
        if (strcmp(args[1], "..") == 0)
        {
            if (currentDirectoryID == 0)
            {
                printf("Already at root directory\n");
                return;
            }
            // Find current directory to get its parent
            for (int i = 0; i < MAX_DIR_AMOUNT; i++)
            {
                if (dirArray[i].repoID == currentDirectoryID)
                {
                    currentDirectoryID = dirArray[i].parentID;
                    printf("Changed to parent directory\n");
                    return;
                }
            }
            printf("Error: Couldn't find parent directory\n");
            return;
        }
        else if (strcmp(args[1], "/") == 0)
        {
            currentDirectoryID = 0;
            printf("Changed to root directory\n");
            return;
        }
        else if (strcmp(args[1], ".") == 0)
        {
            printf("Remaining in current directory\n");
            return;
        }

        // Handle paths
        char *path = strdup(args[1]);
        char *targetName = path;
        short parentID = currentDirectoryID; // Default to current directory for relative paths

        if (path[0] == '/')
        {
            // Absolute path - start from root
            parentID = 0;
            targetName++; // Skip leading slash
        }

        // Find the target directory
        int found = 0;
        short targetID = -1;

        // Check if we have a multi-component path
        char *slash = strchr(targetName, '/');
        if (slash != NULL)
        {
            // For paths like "a/b" or "/a/b"
            *slash = '\0'; // Temporarily terminate at first slash
            parentID = findParentDirID(targetName, parentID);
            targetName = slash + 1;
        }

        if (parentID != -1)
        {
            // Now look for the final directory component
            for (int i = 0; i < MAX_DIR_AMOUNT; i++)
            {
                if (dirArray[i].parentID == parentID &&
                    strcmp(dirArray[i].nomDossier, targetName) == 0)
                {
                    targetID = dirArray[i].repoID;
                    found = 1;
                    break;
                }
            }
        }

        //free(path);

        if (!found)
        {
            printf("Directory '%s' not found\n", args[1]);
            return;
        }

        currentDirectoryID = targetID;
        printf("Changed to directory: %s\n", targetName);
    }
    else if (strcmp(args[0], "chmod") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: chmod <file> <permissions>\n");
            printf("Example: chmod myfile.txt 644\n");
            return;
        }
        unsigned short permissions;
        if (sscanf(args[2], "%ho", &permissions) != 1)
        {
            printf("Invalid permissions format. Use octal (e.g., 755)\n");
            return;
        }
        // Checking if permissions is in correct range && checking result of attempted perms change
        if ((permissions > -1 && permissions < 800) && myChmod(args[1], permissions, currentDirectoryID) == -1)
        {
            printf("Failed to change permissions for %s\n", args[1]);
        }
        else
        {
            printf("Permissions for %s changed to %s\n", args[1], args[2]);
        }
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
    else if (strcmp(args[0], "backup") == 0)
    {
        handleBackupPartition();
    }
    else if (strcmp(args[0], "restore") == 0)
    {
        if (argc < 2)
        {
            printf("Usage: restore <backup_name>\n");
            return;
        }
        handleRestorePartition(args[1]);
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(args[0], "seek") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: seek <filename> <offset> <start|current|end>\n");
            return;
        }

        File *file = myOpen(args[1], currentDirectoryID);
        if (!file)
        {
            printf("File '%s' not found\n", args[1]);
            return;
        }

        int offset = atoi(args[2]);
        int base;

        if (strcmp(args[3], "start") == 0)
            base = 0;
        else if (strcmp(args[3], "current") == 0)
            base = 1;
        else if (strcmp(args[3], "end") == 0)
            base = 2;
        else
        {
            printf("Invalid base. Use: start|current|end\n");
            myClose(file);
            return;
        }

        mySeek(file, offset, base);
        printf("New position: %u/%u\n", file->posSeek, file->size);
        myClose(file);
    }
    else
    {
        printf("Unknown command. Type 'help' for available commands.\n");
    }
}

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
        printf("Usage: fileManager [partitionName].bin\n");
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
    printf("Usage: fileManager [partitionName].bin\n");
    printf("Description:\n");
    printf("  This program is a simple file manager simulation.\n");
    printf("  It allows you to perform various operations on a virtual disk partition.\n\n");
    printf("Example:\n");
    printf("  fileManager maPartition.bin\n");
    printf("    Launch the program and open the specified partition to work on.\n\n");
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

void handleListFolderContents(const char *path)
{
    Directory dirArray[MAX_DIR_AMOUNT];
    if (loadDirBlock(dirArray) == -1)
    {
        printf("Failed to load directory information\n");
        return;
    }

    // Determine target directory
    short targetDirID;
    if (path == NULL || strcmp(path, ".") == 0)
    {
        targetDirID = currentDirectoryID;
    }
    else if (strcmp(path, "..") == 0)
    {
        // Handle parent directory
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            if (dirArray[i].repoID == currentDirectoryID)
            {
                targetDirID = dirArray[i].parentID;
                break;
            }
        }
    }
    else
    {
        // Handle paths
        if (path[0] == '/')
        {
            targetDirID = findParentDirID(path, 0);
        }
        else
        {
            targetDirID = findParentDirID(path, currentDirectoryID);
        }

        if (targetDirID == -1)
        {
            printf("Directory '%s' not found\n", path);
            return;
        }
    }

    // Find target directory
    Directory targetDir;
    int dirFound = 0;
    for (int i = 0; i < MAX_DIR_AMOUNT; i++)
    {
        if (dirArray[i].repoID == targetDirID)
        {
            targetDir = dirArray[i];
            dirFound = 1;
            break;
        }
    }

    if (!dirFound)
    {
        printf("Target directory not found\n");
        return;
    }

    // Display header
    printf("\n=== Contents of %s ===\n",
           path ? path : (targetDirID == 0 ? "root" : "current directory"));

    // List subdirectories (excluding root when in root)
    printf("\n📁 Folders (%d):\n", targetDir.nbSubRepos);
    if (targetDir.nbSubRepos == 0)
    {
        printf("(No subdirectories)\n");
    }
    else
    {
        for (int i = 0; i < MAX_DIR_AMOUNT; i++)
        {
            // Skip root directory (ID 0) when listing root's contents
            if (dirArray[i].parentID == targetDirID &&
                dirArray[i].repoID != 0 && // This excludes root from listing
                strcmp(dirArray[i].nomDossier, "") != 0)
            {
                printf("• %s/\n", dirArray[i].nomDossier);
            }
        }
    }

    // List files (unchanged)
    File fileArray[NUMBER_OF_BLOCK];
    if (loadFileBlock(fileArray) == -1)
    {
        printf("\nFailed to load files information\n");
        return;
    }

    printf("\n📄 Files (%d):\n", targetDir.nbFiles);
    if (targetDir.nbFiles == 0)
    {
        printf("(No files)\n");
    }
    else
    {
        for (int i = 0; i < targetDir.nbFiles; i++)
        {
            int fileIndex = targetDir.files[i];
            if (fileIndex >= 0 && strcmp(fileArray[fileIndex].nom, "") != 0)
            {
                printf("• %s (Size: %d bytes)\n",
                       fileArray[fileIndex].nom,
                       fileArray[fileIndex].size);
            }
        }
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

void handleChmod()
{
    char fileName[MAX_FILES_NAME_SIZE];
    char permStr[5];
    unsigned short permissions;

    printf("Enter file name: ");
    scanf(" %[^\n]", fileName);

    printf("Enter new permissions (e.g., 755, 644): ");
    scanf(" %4s", permStr);

    // Convert permission string to numeric value
    if (sscanf(permStr, "%ho", &permissions) != 1)
    {
        printf("Invalid permissions format. Use octal (e.g., 755)\n");
        return;
    }

    if (myChmod(fileName, permissions, currentDirectoryID) == -1)
    {
        printf("Failed to change permissions for %s\n", fileName);
    }
    else
    {
        printf("Permissions for %s changed to %s\n", fileName, permStr);
    }
}

void handleBackupPartition()
{
    // Get the original name without extension
    char original_name[256];
    char *dot_position;
    strncpy(original_name, PARTITION_NAME, sizeof(original_name) - 1);
    original_name[sizeof(original_name) - 1] = '\0';

    // Remove .bin extension if it exists
    dot_position = strstr(original_name, ".bin");
    if (dot_position != NULL)
    {
        *dot_position = '\0';
    }

    // Create backup name with new format
    char backup_name[267]; // 256 + 12 chr from '_backup.bin' text
    snprintf(backup_name, sizeof(backup_name), "%s_backup.bin", original_name);

    // Open source file
    FILE *source = fopen(PARTITION_NAME, "rb");
    if (source == NULL)
    {
        printf("Error: Could not open source partition\n");
        return;
    }

    // Open destination file
    FILE *dest = fopen(backup_name, "wb");
    if (dest == NULL)
    {
        printf("Error: Could not create backup file\n");
        fclose(source);
        return;
    }

    // Copy file content
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(source);
    fclose(dest);
    printf("Backup created successfully: %s\n", backup_name);
}

void handleRestorePartition(char *backup_name)
{
    // Check if backup file exists
    FILE *backup = fopen(backup_name, "rb");
    if (backup == NULL)
    {
        printf("Error: Backup file does not exist\n");
        return;
    }

    // Close the backup file temporarily
    fclose(backup);

    // Get confirmation from user
    printf("Warning: This will overwrite the current partition. Continue? (y/n): ");
    char response;
    scanf(" %c", &response);
    if (response != 'y' && response != 'Y')
    {
        printf("Restore cancelled\n");
        return;
    }

    // Reopen files for the actual restore
    backup = fopen(backup_name, "rb");
    FILE *dest = fopen(PARTITION_NAME, "wb");

    if (dest == NULL)
    {
        printf("Error: Could not open destination partition\n");
        fclose(backup);
        return;
    }

    // Copy file content
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), backup)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(backup);
    fclose(dest);
    printf("Partition restored successfully from %s\n", backup_name);

    // Reset current directory ID since we've restored a new partition
    currentDirectoryID = 0;

    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    // Force refresh of command prompt by returning immediately
    return;
}