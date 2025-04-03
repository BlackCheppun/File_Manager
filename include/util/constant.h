/**
 * @file constant.h
 * @brief Définition des constantes utilisé
 * On pourra redéfinir ces constant en override avant d'appeler le fichier
 */
/**
 * @brief Nom du fichier dans lequel on va travailler virtuellement dessus
*/
extern char* PARTITION_NAME;

#ifndef BLOCK_SIZE
/// Size of the block in Byte
#define BLOCK_SIZE 4096 
#endif

#ifndef NUMBER_OF_BLOCK
/// Number of block allocated for the data
#define NUMBER_OF_BLOCK 2048
#endif

#ifndef SUPERBLOCK_SIZE
/// Size in byte of the superblock
#define SUPERBLOCK_SIZE 64 
#endif

#ifndef BLOCK_BITMAP_ARRAY_SIZE
/// Define how many blocks there is in our partition
#define BLOCK_BITMAP_ARRAY_SIZE NUMBER_OF_BLOCK
#endif

#ifndef BLOCK_BITMAP_SIZE
/// Size in byte for the whole bitmap
#define BLOCK_BITMAP_SIZE NUMBER_OF_BLOCK*sizeof(unsigned short)
#endif

#ifndef FILE_BLOCK_SIZE
/// Size of the block allocated to register the file
#define FILE_BLOCK_SIZE NUMBER_OF_BLOCK * sizeof(File)
#endif

#ifndef MAX_DIR_AMOUNT
/// Amount of Directories that can be created in the whole partition
#define MAX_DIR_AMOUNT 256
#endif

#ifndef DIR_BLOCK_SIZE
/// Size of the block allocated to register the directory
#define DIR_BLOCK_SIZE MAX_DIR_AMOUNT * sizeof(Directory)
#endif

#ifndef PARTITION_SIZE
/// Size of the all system including sb, data block, file block and bitmap
/// Care the maximum bitmap size is 65536 (unsigned short)
#define PARTITION_SIZE (SUPERBLOCK_SIZE + BLOCK_BITMAP_SIZE + FILE_BLOCK_SIZE + DIR_BLOCK_SIZE + BLOCK_SIZE * NUMBER_OF_BLOCK)
#endif

#ifndef SUPERBLOCK_OFFSET
/// Offset from start that point to the SuperBlock
#define SUPERBLOCK_OFFSET 0
#endif

#ifndef BITMAPBLOCK_OFFSET
/// Offset from start that point to the Block Bitmap
#define BITMAPBLOCK_OFFSET SUPERBLOCK_SIZE
#endif

#ifndef FILEBLOCK_OFFSET
/// Offset from start that point to the File block
#define FILEBLOCK_OFFSET (BITMAPBLOCK_OFFSET + BLOCK_BITMAP_SIZE)
#endif 

#ifndef DIRBLOCK_OFFSET
/// Offset from start that point to the Directory block
#define DIRBLOCK_OFFSET (FILEBLOCK_OFFSET + FILE_BLOCK_SIZE)
#endif 

#ifndef DATABLOCK_OFFSET 
/// Offset from start that point to the Data block
#define DATABLOCK_OFFSET (DIRBLOCK_OFFSET + DIR_BLOCK_SIZE)
#endif 

#ifndef MAX_ENTRIES_DIR 
/// Maximum amount of entries per directory
#define MAX_ENTRIES_DIR 64
#endif 

#ifndef MAX_FILES_NAME_SIZE 
/// Maximum name size for a file
#define MAX_FILES_NAME_SIZE 64
#endif 
//Constantes pour les permissions
#define PERM_OWNER_READ   0400
#define PERM_OWNER_WRITE  0200
#define PERM_OWNER_EXEC   0100
#define PERM_GROUP_READ   0040
#define PERM_GROUP_WRITE  0020
#define PERM_GROUP_EXEC   0010
#define PERM_OTHER_READ   0004
#define PERM_OTHER_WRITE  0002
#define PERM_OTHER_EXEC   0001

