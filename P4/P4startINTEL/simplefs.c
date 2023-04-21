// refernced pg 349 Generic Implementaions of arraylist and Unions.pdf
//useing linear list implemeation
//allocation contiguous
#include "simplefs.h"
#include <stdlib.h>
/*
 * any other includes you require
 */
#include <sys/mman.h> // for mmap
#include <sys/stat.h> // for stat
#include <sys/types.h> // used for size_t
#include <fcntl.h> // for file manipulation 
#include <assert.h> // for assert
#include <fcntl.h> //file control options
#include <stdint.h> // for uint_16
#include <inttypes.h> 
#include <stdio.h> //fprintf 
#include <string.h>
#include <unistd.h>
/*
 * defined constants and structure declarations you need
 */
#define MAX_NUM_FILES 256
#define MAX_FILE_SIZE 2048 //or 32 blocks
#define MAX_FBL 32
#define BLOCK_SIZE 64
#define MAX_BLOCKS 65536
#define MAX_DISK_SIZE 4194304
#define BOOT_BLOCK 0
#define F_HEADER_S 1
#define F_HEADER_E 16
#define DIR_INDEX_BLOCK 17
#define DIRD_BLOCK_S 18
#define DIRD_BLOCK_E 49
#define FREEBL_START 50
#define HEADERS 16
#define ENTRY 8


//for union pdf handout
//(boot block) FCB for us
typedef struct master{
    uint16_t is_inited;
    uint16_t ff_block;
    uint16_t dir_size; //number of valid entryies
    unsigned char extra[BLOCK_SIZE - 2 *sizeof(uint16_t)];
}Master;

// free blocks (arry of free blocks)
typedef struct freebl{
    uint16_t next;
    unsigned char extra[BLOCK_SIZE - 2 *sizeof(uint16_t)];
}Freebl;

// header blocks (start of file, and size of file)
typedef struct header{
    uint16_t f_size;
    uint16_t f_start; // index block num
    unsigned char extra[BLOCK_SIZE - 2 *sizeof(uint16_t)];
}Header;

// directory entry (directory index block) (name and file id)
typedef struct direntry{
    uint16_t fileID;
    unsigned char *name;
    unsigned char extra[BLOCK_SIZE - (sizeof(uint16_t) + 6 *sizeof(unsigned char))];
}DirEntry;

// direct block pointer (maps entrys id to headers)
// keeps track of number of files ie 3 for (header mean first block 12byte)
//(direntery first block 24byte) 
typedef struct dirindex{
    uint8_t h_index;
    uint8_t d_index;
    unsigned char extra[BLOCK_SIZE - 2 * sizeof(uint8_t)];
}DirIndex;

// file content (the block full of file data)
typedef struct filec{
    unsigned char contents[64];
}FileC;

//union struct size of block 64 bytes
typedef union block{
    uint8_t raw[BLOCK_SIZE];
    Freebl fbl;
    Master mst;
    Header *head;
    DirEntry *de;
    DirIndex *di;
    FileC fc;
}Block;
// important data to each instance
typedef struct fsdata{
    /* members you need in the instance-specific structure */
    size_t size; // size of file 
    long capacity; // max number of blocks
    char *stuct_name; // name of file image may not be needed 
    Block *blocks;
} FSData;
/*
 * helper functions needed in your method implementations
 */
static void purge(FSData *fsd)
{
    memset(fsd->blocks,0, fsd->size);
    munmap((char *)fsd->blocks, fsd->size);
}

static void sf_destroy(const SimpleFS *fs) 
{
    FSData *fsd = (FSData *)(fs->self);
    purge(fsd);
    free(fsd->blocks);
    free(fsd);
    free((void *)fs);
}

static void sf_init(const SimpleFS *fs) 
{
    FSData *fsd = (FSData *)(fs->self);
    long f;
    // zero everything
    memset(fsd->blocks,0, fsd->size);
    //set boot block
    fsd->blocks[BOOT_BLOCK].mst.is_inited = 12345;
    fsd->blocks[BOOT_BLOCK].mst.ff_block = FREEBL_START;
    //set up header
    for(long i = F_HEADER_S; i <= F_HEADER_E; i++)
    {
        for(long h = 0; h < HEADERS; h ++)
        {
            fsd->blocks[i].head[h].f_size = 0;
            fsd->blocks[i].head[h].f_start = 0;
        }
    }
    //set index block
    for(long i = 0; i < MAX_NUM_FILES; i ++)
    {
        fsd->blocks[DIR_INDEX_BLOCK].di[i].h_index = 0;
        fsd->blocks[DIR_INDEX_BLOCK].di[i].d_index = 0;
    }
    //set dir entry block
    for(long i = DIRD_BLOCK_S; i <= DIRD_BLOCK_E; i++)
    {
        for(long d = 0 ; d < ENTRY; d ++)
        {
            fsd->blocks[i].de[d].name = NULL;
            fsd->blocks[i].de[d].fileID = 0;
        }
    }
    //set free blocks
    for(long i = FREEBL_START; i < MAX_BLOCKS; i++)
    {
        f = i;
        fsd->blocks[i].fbl.next = f++;
    }
}

static bool sf_create(const SimpleFS *fs, char *name) 
{
    FSData *fsd = (FSData *)(fs->self);

    long fid;
    long index;
    long indexH;

    short unsigned int freeBl;
    short unsigned int next_free;
    freeBl = fsd->blocks[BOOT_BLOCK].mst.ff_block;
    next_free = fsd->blocks[freeBl+32].fbl.next;

    if(name[6] != '\0')
    {
        fprintf(stderr, "name is more the 5 charaters");
        return false;
    }

    for(long i = DIRD_BLOCK_S; i <= DIRD_BLOCK_E; i++)
    {
        for(long j = 0; j < ENTRY; j++)
        {
            if(fsd->blocks[i].de[j].name == (unsigned char *)name)
                return false;
            if(fsd->blocks[i].de[j].name != NULL)
            {
                fid = ((i-18)*16) + (j);
                fsd->blocks[i].de[j].name = (unsigned char *)name;
                fsd->blocks[i].de[j].fileID = fid;
                fsd->blocks[DIR_INDEX_BLOCK].di[i].h_index = fid;
                fsd->blocks[DIR_INDEX_BLOCK].di[i].d_index = fid;
                indexH = (long)fid % 16;
                index = (long)fid / 16 +1;
                fsd->blocks[index].head[indexH].f_size = 2048;
                fsd->blocks[index].head[indexH].f_start = freeBl;
                fsd->blocks[BOOT_BLOCK].mst.ff_block = next_free;
                fsd->blocks[1].head[0].f_size ++;
                return true;
            }
        }
    }
    return false;
}

static bool sf_remove(const SimpleFS *fs, char *name) 
{
    FSData *fsd = (FSData *)(fs->self);

    long fid;
    long index;
    long indexH;

    short unsigned int next_free;

    if(name[6] != '\0')
    {
        fprintf(stderr, "name is more the 5 charaters");
        return false;
    }

    for(long i = DIRD_BLOCK_S; i <= DIRD_BLOCK_E; i++)
    {
        for(long j = 0; j < ENTRY; j++)
        {
            if(fsd->blocks[i].de[j].name == (unsigned char *)name)
            {
                fsd->blocks[i].de[j].name = NULL;
                fid = (long)fsd->blocks[i].de[j].fileID;
                fsd->blocks[i].de[j].fileID = 0;
                indexH = fid % 16;
                index = fid / 16 +1;
                fsd->blocks[DIR_INDEX_BLOCK].di[i].h_index = 0;
                fsd->blocks[DIR_INDEX_BLOCK].di[i].d_index = 0;
                fsd->blocks[index].head[indexH].f_size = 0;
                next_free = fsd->blocks[index].head[indexH].f_start;
                fsd->blocks[BOOT_BLOCK].mst.ff_block = next_free;
                fsd->blocks[1].head[0].f_size --;
                fsd->blocks[index].head[indexH].f_start = 0;
                return true;
            }
        }
    }
    return false;
}

static bool sf_write(const SimpleFS *fs, char *name, char *content) 
{
       FSData *fsd = (FSData *)(fs->self);

    long fid;
    long start;
    long index;
    long indexH;

    int fd;
    struct stat flbuf;
    fd = open(content, O_RDWR, S_IRWXU);

    if(fstat(fd,&flbuf) == -1)
    {
        fprintf(stderr, "Unable to get file size");
        close(fd);
        return false;
    }
    close(fd);
    if(flbuf.st_size > MAX_FILE_SIZE)
    {
        return false;
    }

    FILE *fp;
    fp = fopen(content, "r");

    for(long i = DIRD_BLOCK_S; i <= DIRD_BLOCK_E; i++)
    {
        for(long j = 0; j < ENTRY; j++)
        {
            if(fsd->blocks[i].de[j].name == (unsigned char *)name)
            {
               fid = fsd->blocks[i].de[j].fileID;
               indexH = fid % 16;
               index = fid / 16 +1;
               start = fsd->blocks[index].head[indexH].f_start;
               for(long i = start; i < (start +32); i++)
               {
                    //off set the next values in the block
                    for(long j = 2; j < BLOCK_SIZE; j++)
                    {
                        fsd->blocks[i].raw[j] = fgetc(fp);    
                    }
               }
               fclose(fp);
               return true;
            }
        }
    }
    return false;
}

static bool sf_read(const SimpleFS *fs, char *name, char *content) 
{
    FSData *fsd = (FSData *)(fs->self);

    long fid;
    long start;
    long index;
    long indexH;

    int fd;
    struct stat flbuf;
    fd = open(content, O_RDWR, S_IRWXU);

    if(fstat(fd,&flbuf) == -1)
    {
        fprintf(stderr, "Unable to get file size");
        close(fd);
        return false;
    }
    close(fd);

    FILE *fp;
    fp = fopen(content, "w");

    for(long i = DIRD_BLOCK_S; i <= DIRD_BLOCK_E; i++)
    {
        for(long j = 0; j < ENTRY; j++)
        {
            if(fsd->blocks[i].de[j].name == (unsigned char *)name)
            {
               fid = fsd->blocks[i].de[j].fileID;
               indexH = fid % 16;
               index = fid / 16 +1;
               start = fsd->blocks[index].head[indexH].f_start;
               for(long i = start; i < (start +32); i++)
               {
                    //off set the next values in the block
                    for(long j = 2; j < BLOCK_SIZE; j++)
                    {
                        if(fsd->blocks[start].raw[j] == 0)
                        {
                            content[0] = '\0';
                            fclose(fp);
                            return true;
                        }
                        if(fsd->blocks[i].raw[j] != 0)
                        {
                            fputc(fsd->blocks[i].raw[j], fp);    
                        }   
                    }
               }
               fclose(fp);
               return true;
            }
        }
    }
    return false;
}

static bool sf_list(const SimpleFS *fs, char *filenames) 
{
    FSData *fsd = (FSData *)(fs->self);



    for(long i = DIRD_BLOCK_S; i <= DIRD_BLOCK_E; i++)
    {
        for(long j = 0; j < ENTRY; j++)
        {
            if(fsd->blocks[i].de[j].name != NULL)
            {
                strcat(filenames, (char *)fsd->blocks[i].de[j].name);
                strcat(filenames, ", ");
            }
        }
    }
    return true;
}

static bool sf_info(const SimpleFS *fs, char *information) 
{
    FSData *fsd = (FSData *)(fs->self);
    if(fsd->blocks[BOOT_BLOCK].mst.is_inited == 12345)
    {
        sprintf(information, "Directory Exists");
        sprintf(information,"Number of  files in Directory: %ud", fsd->blocks[1].head[0].f_size);
        sprintf(information,"Disk has space for: %ud Files",256-fsd->blocks[1].head[0].f_size);
        return true;
    }

    sprintf(information, "Directory doesn't exists");
    return false;
}

static bool sf_dump(const SimpleFS *fs, char *dumpinfo) 
{
    FSData *fsd = (FSData *)(fs->self);

    for(long i = F_HEADER_S; i < F_HEADER_E; i++)
    {
        for(long j = 0; j < HEADERS; j++)
        {
            if(i == 1 && j == 0)
            {
                sprintf(dumpinfo, "Directory size: %ud", fsd->blocks[i].head[j].f_size);
            }
            if(fsd->blocks[i].head[j].f_size != 0)
            {
                sprintf(dumpinfo, "File Start %ud \n", fsd->blocks[i].head[j].f_start);
                sprintf(dumpinfo, "File Size %ud \n", fsd->blocks[i].head[j].f_size);
            }
        }
    }

    return true;
}

static bool sf_block(const SimpleFS *fs, int block, char *blockinfo) 
{
    FSData *fsd = (FSData *)(fs->self);


    if(block == 0)
    {
        sprintf(blockinfo, "%ud, ",fsd->blocks[block].mst.is_inited);
        sprintf(blockinfo, "%ud, ",fsd->blocks[block].mst.ff_block);
        return true;
    }

    if(block > 0 && block < 17)
    {
        for(long i = 0; i < HEADERS; i++)
        {
            sprintf(blockinfo,"%ud, ", fsd->blocks[block].head[i].f_start);
            sprintf(blockinfo,"%ud, ", fsd->blocks[block].head[i].f_size);
        }
        return true;
    }

    if(block == 17)
    {
        for(long i = 0; i < MAX_NUM_FILES; i++)
        {
            sprintf(blockinfo, "%ud, ", fsd->blocks[block].di[i].h_index);
            sprintf(blockinfo, "%ud, ", fsd->blocks[block].di[i].d_index);
        }
        return true;
    }

    if(block > 17 && block < 50)
    {
        for(long i = 0; i < ENTRY; i++)
        {
            sprintf(blockinfo,"%s, ", (char *)fsd->blocks[block].de[i].name);
            sprintf(blockinfo,"%ud, ", fsd->blocks[block].de[i].fileID);
        }   
        return true;
    }

    if(block > 49)
    {
        for(long i = 0; i < MAX_NUM_FILES; i++)
        {
            sprintf(blockinfo,"%ud, ", fsd->blocks[block].di[i].h_index);
            sprintf(blockinfo,"%ud, ", fsd->blocks[block].di[i].d_index);
        }
        return true;
    }

    return false;
}

static const SimpleFS template = {
    NULL, sf_destroy, sf_init, sf_create, sf_remove, sf_write, sf_read,
    sf_list, sf_info, sf_dump, sf_block
};
//pg 354 pg 568 os book
const SimpleFS *SimpleFS_create(char *filename) 
{
    int fd;
    struct stat stbuf;
    fd = open(filename, O_RDWR, S_IRWXU);

    if(fstat(fd,&stbuf) == -1)
    {
        fprintf(stderr, "Unable to get file size and open");
        close(fd);
        return NULL;
    }

    SimpleFS *fs = (SimpleFS *)malloc(sizeof(SimpleFS));
    if(fs != NULL)
    {
        FSData *fsd = (FSData *)malloc(sizeof(FSData));
        if(fsd != NULL)
        {
            //block of memory big array of bytes
            Block *fmem_array = mmap(NULL,stbuf.st_size , PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);
            if(fmem_array != NULL)
            {
                fsd->capacity = MAX_BLOCKS;
                fsd->size = stbuf.st_size;
                fsd->stuct_name = filename;
                fsd->blocks = (Block *)(fmem_array);
                *fs = template;
                fs->self = fsd;
            } 
            else
            {
                free(fsd);
                free(fs);
                fs = NULL;
            }
        }
        else
        {
            free(fs);
            fs = NULL;
        }
    }
    close(fd);
    return fs;
}
