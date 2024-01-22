/***********************************************
*
* Pràctica 'The Shooter'
* Sistemes Operatius Avançats, Curs 2021-22
*
* @author Oscar Julian Ponte (oscar.julian)
*
* info.h
************************************************/

#ifndef _info_H_
#define _info_H_

/*
    LLIBRERIES
*/

//Llibreries del sistema
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
    CONSTANTS
*/

#define Offs_EXT 1024
#define Offs_FAT12 36
#define EXT2_FS 0xEF53

/*
    TIPUS PROPIS
*/

typedef struct
{
  unsigned short size;
  int numInodes;
  unsigned short firstInode;
  int inodesGroup;
  int freeInodes;
} InfoInodeEXT2;

typedef struct
{
  int size;
  int reservedBlocks;
  int freeBlocks;
  int totalBlocks;
  int firstBlock;
  int groupBlocks;
} InfoBlockEXT2;

typedef struct
{
  char volumeName[16];
  char lastChecked[100];
  char editedLast[100];
  char writtenLast[100];
} InfoVolumeEXT2;

typedef struct
{
  char systemName[100];
  unsigned short size;
  unsigned short sectorsXCluster;
  unsigned short reservedSectors;
  unsigned short numFATs;
  unsigned short maxRootEntries;
  unsigned short sectorsXFAT;
  unsigned short countofClusters;
  unsigned short BPB_FATSz16;
  unsigned short BPB_BytsPerSec;
  unsigned short FirstRootDirSecNum;
  char label[100];
} InfoFAT16;

/*
    PROCEDIMENTS & FUNCIONS
*/

InfoInodeEXT2 readInodeEXT2(FILE *file);
InfoBlockEXT2 readBlockEXT2(FILE *file);
InfoVolumeEXT2 readVolumeEXT2(FILE *file);
InfoFAT16 readInfoFAT16(FILE *file);

#endif /* info.h */
