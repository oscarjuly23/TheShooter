/***********************************************
*
* Pràctica 'The Shooter'
* Sistemes Operatius Avançats, Curs 2021-22
*
* @author Oscar Julian Ponte (oscar.julian)
*
* findOrDelete.h
************************************************/

#ifndef _findOrDelete_H_
#define _findOrDelete_H_

//Llibreries del sistema
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <time.h>

#include "./info.h"

#define Offs_SB 1024

/*
    TIPUS PROPIS
*/

typedef struct {
    unsigned char i_mode; // 16bit value used to indicate the format of the described file and the access rights
    unsigned char i_uid; // 16bit user id associated with the file.
    int i_size; // this represents the lower 32-bit of the file size.
    int i_atime; // 32bit value representing the number of seconds since january 1st 1970 of the last time this inode was accessed.
    int i_ctime; // 32bit value representing the number of seconds since january 1st 1970, of when the inode was created.
    int i_mtime; // 32bit value representing the number of seconds since january 1st 1970, of the last time this inode was modified.
    int i_dtime; // 32bit value representing the number of seconds since january 1st 1970, of when the inode was deleted.
    unsigned char i_gid; // 16bit value of the POSIX group having access to this file.
    unsigned char i_links_count; // 16bit value indicating how many times this particular inode is linked (referred to).
    int  unsigned char_blocks; // 32-bit value representing the total number of 512-bytes blocks reserved to contain the data of this inode
    int i_flags; // 32bit value indicating how the ext2 implementation should behave when accessing the data for this inode.
    int i_osd1; // 32bit OS dependant value.
    int i_block[15]; // 15 x 32bit block numbers pointing to the blocks containing the data for this inode.
    int i_generation; // 32bit value used to indicate the file version (used by NFS).
    int i_file_acl; // 32bit value indicating the block number containing the extended attributes.
    int i_dir_acl; // 32bit value contains the high 32 bits of the 64bit file size.
    int i_faddr; // 32bit value indicating the location of the file fragment.
    unsigned char i_osd2[12]; //96bit OS dependant structure.
} InodeInfo;

typedef struct{
    int bg_block_bitmap; //32bit block id of the first block of the “block bitmap” for the group represented.
    int bg_inode_bitmap; //32bit block id of the first block of the “inode bitmap” for the group represented.
    int bg_inode_table; // 32bit block id of the first block of the “inode table” for the group represented.
    short bg_free_blocks_count; // 16bit value indicating the total number of free blocks for the represented group.
    short bg_free_inodes_count; // 16bit value indicating the total number of free inodes for the represented group.
    short bg_used_dirs_count; // 16bit value indicating the number of inodes allocated to directories for the represented group.
    short bg_pad; // 16bit value used for padding the structure on a 32bit boundary.
    unsigned char bg_reserved[12]; // 12 bytes of reserved space for future revisions.
}BlockGroupDescriptor;

typedef struct{
    int inode; // 32bit inode number of the file entry. A value of 0 indicate that the entry is not used.
    unsigned short rec_len; // 16bit unsigned displacement to the next directory entry from the start of the current directory entry.
    char name_len; // 8bit unsigned value indicating how many bytes of character data are contained in the name.
    char file_type; // 8bit unsigned value used to indicate file type.
    //char name[255];
}DIR_StructEXT2;

/*
    PROCEDIMENTS & FUNCIONS
*/

int findOrDeleteEXT2(char *fileName, FILE *file, int nInode, bool delete);
int findOrDeleteFAT16(char *fileName, FILE *file, unsigned short RootClus, bool delete);


#endif /* findOrDelete.h */
