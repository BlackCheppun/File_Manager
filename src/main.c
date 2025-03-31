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

char* PARTITION_NAME;
void pauseEnter();
void showHelpMessage();
void displayMenu();
void handleCreateFile();
void handleDeleteFile();
void handleRenameFile();
void handleReadFile();
void handleModifyFile();
void handleListFile();
void handleFormatPartition();
void handleVisualizePartition();

int main(int argc, char** argv){
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
    // Ask for format
    displayMenu();

    return 0;

}


void showHelpMessage(){
    printf("Usage: fileManager [partitionName]\n");
    printf("Description:\n");
    printf("  This program is a simple file manager simulation.\n");
    printf("  It allows you to perform various operations on a virtual disk partition.\n\n");
    printf("Example:\n");
    printf("  fileManager maPartition.bin\n");
    printf("    Launch the program and open the specified partition to work on.\n\n");
}

void displayMenu() {
    int choice;
    do {
        printf("\nMenu: (working on : %s)\n",PARTITION_NAME);
        printf("1. Create File\n");
        printf("2. Delete File\n");
        printf("3. Rename File\n");
        printf("4. Read File\n");
        printf("5. Modify File\n");
        printf("6. List Files\n");
        printf("7. Format Partition\n");
        printf("8. Visualize Partition\n");
        printf("9. Exit\n");
        printf("Enter your choice: \n");

        scanf("%d", &choice);

        switch (choice) {
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
                handleListFile();
                break;
            case 7:
                handleFormatPartition();
                break;
            case 8:
                handleVisualizePartition();
                break;
            case 9:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please enter a number between 1 and 9.\n");
        }
    } while (choice != 9);
}

void handleCreateFile(){
    char buf[128] = "";
    char bigBuffer[2049]= "";
    File* tmp;
    printf("File name : \n");
    scanf(" %s",buf);
    if((tmp = myOpen(buf))==NULL){
        printf("Something went wrong when creating file.\n");
        exit(1);
    }
    printf("Content (2048 char max per input) (terminate writing with a dot '.' on a new line): \n");
    while (bigBuffer[0] != '.')
    {
        scanf(" %[^\n]s",bigBuffer);
        bigBuffer[2048] = '\0';
        if (myWrite(tmp,bigBuffer,strlen(bigBuffer))== -1){
            printf("Error when writing to file");
            exit(1);
        }
    }
    printf("Successfully created file : %s\n",tmp->nom);
    free(tmp);
}


void handleDeleteFile(){
    char buf[128] = "";
    printf("Specify which file to delete : ");
    scanf(" %[^\n]s",buf);
    if (strlen(buf) > MAX_FILES_NAME_SIZE)
    {
        printf("Filename cannot exceed %u char",MAX_FILES_NAME_SIZE);
        return;
    }
    if (myDelete(buf)==-1)
    {
        printf("Error when deleting file (does not exist or bad match), case sensitive\n");
        return;
    }
    printf("Successfully deleted file : %s",buf);
}
void handleRenameFile(){
    char buf[128] = "";
    char rename[65] = "";
    printf("Specify which file to rename: \n");
    scanf(" %[^\n]s",buf);
    printf("Renaming to : \n");
    scanf(" %[^\n]s",rename);
    if (strlen(buf) > MAX_FILES_NAME_SIZE || strlen(rename) > MAX_FILES_NAME_SIZE)
    {
        printf("Filename cannot exceed %u char",MAX_FILES_NAME_SIZE);
        return;
    }
    if (myRename(buf,rename)==-1)
    {
        printf("Error when renaming file (does not exist or bad match), case sensitive\n");
        return;
    }
    printf("Successfully renamed file from : %s to : %s ",buf,rename);
}

void handleReadFile(){
    char buf[128] = "";
    int nbToRead= 0;
    int offset= 0;
    int base= 0;
    int totalRead;
    File* tmp;
    printf("File name : ");
    scanf(" %s",buf);
    if((tmp = myOpen(buf))==NULL){
        printf("Something went wrong when opening file.\n");
        exit(1);
    }
    printf("\nHow much char (-1 to show all) : ");
    scanf(" %d",&nbToRead);
    if (nbToRead != -1)
    {
        printf("\nWhich base : \n");
        printf("1. Start\n");
        printf("2. Specific\n");
        printf("3. End\n");
        scanf(" %d",&base);
        if (base > 3 || base < 1)
        {
            free(tmp);
            printf("Error when choosing Base");
            return;
        }
        
        printf("Offset  : \n");
        scanf(" %d",&offset);
        char* bufferMsg = (char*)malloc((nbToRead+1)*sizeof(nbToRead));
        mySeek(tmp,offset,base-1);

        if ((totalRead = myRead(tmp,bufferMsg,nbToRead)) == -1)
        {
            free(tmp);
            free(bufferMsg);
            printf("Error when reading file");
            return;
        }
        bufferMsg[totalRead] = '\0';
        printf("%s",bufferMsg);
    }else{
        char * msg = (char*) malloc((1+tmp->size)*sizeof(char));
        if ((totalRead = myRead(tmp,msg,tmp->size))==-1)
        {
            free(tmp);
            free(msg);
            printf("Error when reading the whole file\n");
            return;
        }
        msg[totalRead] = '\0';
        printf("%s\n",msg);
        free(msg);
    }
    free(tmp);
    pauseEnter();
}

void handleModifyFile(){
    char buf[128] = "";
    char bufMsg[2049] = "";

    int offset= 0;
    int base= 0;
    File* tmp;
    printf("File name : ");
    scanf(" %s",buf);
    if((tmp = myOpen(buf))==NULL){
        printf("Something went wrong when opening file.\n");
        exit(1);
    }
    printf("\nWhich base : \n");
    printf("1. Start\n");
    printf("2. Specific\n");
    printf("3. End\n");
    scanf(" %d",&base);
    if (base > 3 || base < 1)
    {
        free(tmp);
        printf("Error when choosing Base");
        return;
    }
    
    printf("Offset  : \n");
    scanf(" %d",&offset);
    
    mySeek(tmp,offset,base-1);
    
    printf("Content (2048 char max per input) (terminate writing with a dot '.' on a new line): \n");
    while (bufMsg[0] != '.')
    {
        scanf(" %[^\n]s",bufMsg);
        bufMsg[2048] = '\0';
        if (myWrite(tmp,bufMsg,strlen(bufMsg))== -1){
            free(tmp);
            printf("Error when writing to file");
            return;
        }
    }
    free(tmp);
    pauseEnter();
}

void handleListFile(){
    SuperBlock sb;
    loadSuperBlock(&sb);
    File array[NUMBER_OF_BLOCK];
    loadFileBlock(array);
    int i = 0;
    int nbFile = sb.totalFile - sb.nbFileDispo;
    printf("\nFiles in %s :\n",PARTITION_NAME);
    while (i < nbFile)
    {
        printf("-> %s\n",array[i].nom);
        i++;
    }
    pauseEnter();
}
void handleFormatPartition(){
    char accept = 'a';
    printf("This action will erase all file in %s, are you sure ? (Takes 30-50 seconds) (Y/N) \n",PARTITION_NAME);
    scanf(" %c",&accept);
    if (accept == 'Y' || accept == 'y')
    {
        if (myFormat(PARTITION_NAME)==-1)
        {
            printf("Something went wrong when formatting, perhaps we didn't recognized our proprietary label, please delete manually.\n");
            exit(1);
        }
        printf("Successfully formated partition.\n");
    }
    pauseEnter();
}
void handleVisualizePartition(){
    SuperBlock sb;
    loadSuperBlock(&sb);
    BlockBitmap bbmp;
    loadBlockBitmap(&bbmp);
    File array[NUMBER_OF_BLOCK];
    loadFileBlock(array);
    printSB(sb);
    printBBMP(bbmp);
    printf("Data disk usage : %.3f%%\n", (float)(sb.totalBlock-sb.nbBlockDispo)/(float)sb.totalBlock);
    int nbFile = sb.totalFile - sb.nbFileDispo;
    int cpt = 0;
    for (int i = 0; i < nbFile; i++)
    {
        cpt = array[i].posInBlockBMP;
        printf("\tFile : %s\n",array[i].nom);
        printf("\t\tIndex blocks : ");
        while (cpt != USHRT_MAX)
        {
            printf("%u ",cpt);
            cpt = bbmp.bmpTab[cpt];
            if (cpt != USHRT_MAX)
            {
                printf("-> ");
            }
        }
        printf("\n\t\tSize : %uB\n",array[i].size);
    }
    
    pauseEnter();
}

void pauseEnter(void)
{
    printf("Press Enter to continue...");
    fflush(stdout);
    char enter;
    scanf("%c", &enter); 
    while (getchar() != '\n');
}