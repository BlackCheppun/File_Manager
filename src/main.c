#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include "process/Fichier.h"
#include "process/Partition.h"
#include "data/TypeSuperBloc.h"
#include "data/TypeDossier.h"
#include "util/constant.h"
#include "util/Tools.h"

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
    /*
    File* f = myOpen("coucou");
    
    if (f != NULL)
    {
        char* msg = "je suis un bout de texte!";
        myWrite(f,msg,strlen(msg));
        mySeek(f,-10,MYSEEK_CUR);  
        char* msg1 = "abcdefghijklmnopqrstuvwxyz";
        myWrite(f,msg1,strlen(msg1));
    }
    
    
    File* file = myOpen("papi");  
    char* bigmsg = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque eget risus quam. Sed sed tortor sit amet velit condimentum condimentum. Nunc lorem orci, ullamcorper id rhoncus nec, vehicula id risus. Sed iaculis neque maximus mauris lacinia hendrerit. Vestibulum tincidunt eget nunc vitae consectetur. Fusce aliquam ex ac mauris suscipit consectetur. Pellentesque arcu sem, mattis et commodo quis, molestie id arcu. Sed accumsan porttitor justo, sed luctus enim fermentum tempus. Maecenas interdum enim erat, a blandit nulla facilisis nec. Integer malesuada turpis massa, eget varius lacus viverra sed. Curabitur ultrices felis ut odio elementum pulvinar. Aenean sed sodales velit. Cras vitae pellentesque urna.Nam non elit in purus tristique viverra sit amet non ipsum. Sed a neque eget nibh sagittis euismod. Donec sollicitudin porta sapien. Morbi blandit a magna vitae eleifend. Donec pharetra hendrerit diam, ac consequat metus rutrum pharetra. Etiam non magna erat. Suspendisse sit amet suscipit dolor. In semper, libero id porta bibendum, justo augue sagittis mi, vel consectetur metus nunc quis ex. Ut ex tortor, vestibulum vitae libero eu, iaculis sodales erat. Suspendisse dignissim, justo ut blandit suscipit, est lacus finibus magna, in efficitur arcu nulla in sapien. Nunc non convallis tellus, et ullamcorper purus. Curabitur nec dapibus lectus. Integer tincidunt, libero id tristique scelerisque, purus nunc laoreet odio, ut congue risus ipsum non dolor. Nullam ornare enim id nisi porttitor, sit amet maximus dui ullamcorper.Curabitur a odio a nibh bibendum porttitor. Nullam in enim fermentum, eleifend magna ultricies, ornare ligula. Nunc imperdiet tincidunt velit nec suscipit. Phasellus blandit vehicula enim, non blandit sem sollicitudin a. Sed vulputate ipsum mi, vel aliquam risus feugiat sed. Nullam ex massa, laoreet et ultrices at, cursus id turpis. Phasellus magna enim, efficitur sit amet aliquam vel, feugiat quis arcu. Morbi sed elit vel nulla blandit pretium in sed elit. Sed faucibus interdum posuere.Nam posuere eros at dui malesuada, ut sollicitudin turpis luctus. Pellentesque ac est eget ante imperdiet rhoncus non viverra mauris. Vestibulum libero ligula, aliquet eget euismod at, mollis id massa. Nullam pretium imperdiet lacus, sit amet sagittis dolor semper fringilla. Curabitur vehicula convallis lacinia. Suspendisse dui nibh, placerat ut ultrices nec, elementum id augue. Sed purus ipsum, maximus id quam eu, scelerisque imperdiet ipsum. Mauris sollicitudin efficitur tincidunt. Phasellus sagittis rhoncus lectus eget varius. Quisque magna lacus, sagittis finibus lorem nec, fringilla faucibus erat. Aliquam quis tempor enim.Etiam imperdiet ultrices ligula, at egestas magna tincidunt vitae. Maecenas in convallis libero. Vivamus in lectus id sem eleifend tincidunt. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Curabitur euismod sed sem sit amet semper. Nullam et ante facilisis, malesuada purus et, finibus libero. Curabitur eget nisi luctus, pretium urna sit amet, pretium sem. Nunc ut ultrices ipsum, non ultricies nibh. In risus velit, mattis a rhoncus id, dictum in ipsum. Phasellus ac ultrices libero, eget tempus sapien.Pellentesque sit amet justo in urna dignissim malesuada rutrum id risus. Nam scelerisque lacus eget dolor iaculis, nec commodo sem laoreet. Donec quis luctus magna, in posuere sapien. Vivamus in ipsum sagittis, pretium leo sit amet, posuere libero. Mauris tincidunt felis purus, et eleifend est dapibus in. Maecenas semper vitae turpis ac gravida. Nullam leo neque, ornare eget ligula ut, tempor tristique nisl. Praesent iaculis vitae libero sit amet vestibulum.Donec non aliquet erat. Duis mattis magna in lectus aliquam, in elementum est fermentum. Nullam sollicitudin, leo a lacinia tempor, erat turpis pharetra orci, eu accumsan mauris velit quis enim. Fusce et leo nec massa lobortis convallis eget sed sem. Vestibulum tincidunt enim pretium, blandit nulla at, fringilla metus. Curabitur vitae elit sit amet augue mollis eleifend. Ut blandit convallis tellus non odio.NEWBLOCKHERE";
    if (file == NULL){
        printf("Je suis NULL\n");
    }else{
        myWrite(file,bigmsg,strlen(bigmsg));
    }
    File* file2 = myOpen("monSuperTexte");
    
    if (file2 != NULL)
    {
        char* msg = "Rose are red, Violet are blue.";
        myWrite(file2,msg,strlen(msg));
        char* msg1 = "Bring dat stick and smack this.";
        myWrite(file2,msg1,strlen(msg1));
    }
    myWrite(file,bigmsg,strlen(bigmsg));
    char smolbuf[4096] = {'a'};
    myWrite(file,smolbuf,strlen(smolbuf));
    free(f);
    free(file);
    free(file2);
    
    */
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