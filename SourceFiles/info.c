/***********************************************
*
* Pràctica 'The Shooter'
* Sistemes Operatius Avançats, Curs 2021-22
*
* @author Oscar Julian Ponte (oscar.julian)
*
* info.c
* @Proposit: Extreure informació dels volums
************************************************/

//Llibreries del sistema
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//Llibreries propies
#include "../HeaderFiles/info.h"

/*
    PROCEDIMENTS & FUNCIONS
*/

/***********************************************
*
* @Finalitat: Llegir i guardar la informació del Inode d'un volum EXT2
* @Parametres: in: FILE = volum EXT2 indicat per l'usuari
* @Retorn:     out: infoInodeEXT2: Estructura amb la informació dels inodes.
*
************************************************/
InfoInodeEXT2 readInodeEXT2(FILE *file) {
  InfoInodeEXT2 infoInodeEXT2;

  // s_inode_size: Valor de 16bit que indica el tamany de la estructura de inodes.
  fseek(file, Offs_EXT + 88, SEEK_SET); // Offset: 88 bytes, Size: 2 bytes
  fread(&infoInodeEXT2.size, 1, 2, file);

  // s_inodes_ count: Valor de 32bit que indica el nombre total de inodes
  fseek(file, Offs_EXT + 0, SEEK_SET); // Offset: 0 bytes, Size: 4 bytes
  fread(&infoInodeEXT2.numInodes, 1, 4, file);

  // s_first_ino: Valor de 32bit utilitzat com a index per al primer inode usable per a fitxers estandars.
  fseek(file, Offs_EXT + 84, SEEK_SET); // Offset: 84 bytes, Size: 2 bytes
  fread(&infoInodeEXT2.firstInode, 1, 2, file);

  // s_inodes_per_group: Valor de 32bit que indica el nombre total de inodes per grup.
  fseek(file, Offs_EXT + 40, SEEK_SET); // Offset: 40 bytes, Size: 4 bytes
  fread(&infoInodeEXT2.inodesGroup, 1, 4, file);

  // s_free_inodes_count: Valor de 32bit que indica el nombre total de blocks de inodes lliures
  fseek(file, Offs_EXT + 16, SEEK_SET); //Offset: 16 bytes, Size: 4 bytes
  fread(&infoInodeEXT2.freeInodes, 1, 4, file);
  
  return infoInodeEXT2;
}

/***********************************************
*
* @Finalitat: Llegir i guardar la informació del Block d'un volum EXT2
* @Parametres: in: FILE = volum EXT2 indicat per l'usuari
* @Retorn:     out: infoBlockEXT2: Estructura amb la informació dels blocs.
*
************************************************/
InfoBlockEXT2 readBlockEXT2(FILE *file) {
  InfoBlockEXT2 infoBlockEXT2;

  // s_log_block_size: El tamany del bloc ve indicat utilitzant aquest valor de 32bit com a nombre de bits que es shiften a l'esquerra fins arribar a 1024.
  fseek(file, Offs_EXT + 24, SEEK_SET); // Offset: 24 bytes, Size 4 bytes
  fread(&infoBlockEXT2.size, 1, 4, file); // value as the number of bits to shift left the value 1024

  //s_r_blocks_count: Valor de 32 bit que indica el nombre total de blocs reservats per al ús del super usuari.
  fseek(file, Offs_EXT + 8, SEEK_SET); // Offset: 8 bytes, Size: 4 bytes
  fread(&infoBlockEXT2.reservedBlocks, 1, 4, file);

  // s_free_blocks_count: Valor de 32bit que indica el nombre total de blocs lliures (inclueix el nombre de blocs reservats)
  fseek(file, Offs_EXT + 12, SEEK_SET); // Offset: 12 bytes, Size: 4 bytes
  fread(&infoBlockEXT2.freeBlocks, 1, 4, file);

  //s_blocks_count: Valor de 32bit que indica el nombre total de blocs del sistema (inclueix els utilitzats, els lliures i els reservats).
  fseek(file, Offs_EXT + 4, SEEK_SET); // Offset: 4 bytes, Size: 4 Bytes
  fread(&infoBlockEXT2.totalBlocks, 1, 4, file);

  // s_first_data_block: Valor de 32bit que identifica el primer bloc de dades
  fseek(file, Offs_EXT + 20, SEEK_SET); // Offset: 20 bytes, Size: 4 bytes
  fread(&infoBlockEXT2.firstBlock, 1, 4, file);

  // s_blocks_per_group: Valor de 32bit que indica  el nombre total de blocks per grup.
  fseek(file, Offs_EXT + 32, SEEK_SET); // Offset: 32 bytes, Size: 4 bytes
  fread(&infoBlockEXT2.groupBlocks, 1, 4, file);

  return infoBlockEXT2;
}

/***********************************************
*
* @Finalitat: Llegir i guardar la informació del Volume d'un volum EXT2
* @Parametres: in: FILE = volum EXT2 indicat per l'usuari
* @Retorn:     out: infoVolumeEXT2: Estructura amb la informació del volum.
*
************************************************/
InfoVolumeEXT2 readVolumeEXT2(FILE *file) {
  InfoVolumeEXT2 infoVolumeEXT2;
  int buffer;
  time_t t;
  struct tm *tm;

  // s_volume_name: Valor de 16 bytes de nom (normalment no s'utilitza)
  fseek(file, Offs_EXT + 120, SEEK_SET); // Offset: 120 bytes, Size: 16 bytes
  fread(infoVolumeEXT2.volumeName, 1, 16, file);

  // s_lastcheck: Ultima vegada que s'ha fet check del file system (Unix time, POSIX).
  fseek(file, Offs_EXT + 64, SEEK_SET); // Offset: 64 bytes, Size: 4 bytes
  fread(&buffer, 1, 4, file);
  t = buffer;
  tm = localtime(&t);
  strftime(infoVolumeEXT2.lastChecked, 100, "%c", tm);

  // s_mtime: Ultima vegada que s'ha muntat aquest file system (Unix time, POSIX)
  fseek(file, Offs_EXT + 44, SEEK_SET); // Offset: 44 bytes, Size: 4 bytes
  fread(&buffer, 1, 4, file);
  t = buffer;
  tm = localtime(&t);
  strftime(infoVolumeEXT2.editedLast, 100, "%c", tm);

  // s_wtime: Ultima vegada que s'ha realitzar un accés de escriptura al file system (Unix time, POSIX)
  fseek(file, Offs_EXT + 48, SEEK_SET); // Offset: 48 bytes, Size: 4 bytes
  fread(&buffer, 1, 4, file);
  t = buffer;
  tm = localtime(&t);
  strftime(infoVolumeEXT2.writtenLast, 100, "%c", tm);

  return infoVolumeEXT2;
}

/***********************************************
*
* @Finalitat: Llegir i guardar la informació d'un volum FAT16
* @Parametres: in: FILE = volum FAT16 indicat per l'usuari
* @Retorn:     out: infoFAT16: Estructura amb la informació de tot el volum.
*
************************************************/
InfoFAT16 readInfoFAT16(FILE *file) {
  InfoFAT16 infoFAT16;

  unsigned short BPB_FATSz16;
  unsigned short BPB_TotSec16;
  char BPB_NumFATs;
  unsigned short BPB_RootEntCnt;
  unsigned short BPB_RsvdSecCnt;
  unsigned short BPB_BytsPerSec;
  unsigned short RootDirSectors;
  unsigned short DataSec;
  unsigned short CountofClusters;
  char BPB_SecPerClus;
  int FirstRootDirSecNum;

  // BS_OEMName: It is only a name string.
  fseek(file, 3, SEEK_SET); // Offset: 3 bytes, Size: 8 bytes
  fread(infoFAT16.systemName, 1, 8, file);

  // If(BPB_FATSz16 != 0)
  // FATSz = BPB_FATSz16;
  // Else
  // FATSz = BPB_FATSz32;
  // If(BPB_TotSec16 != 0)
  // TotSec = BPB_TotSec16;
  // Else
  // TotSec = BPB_TotSec32;

  // BPB_FATSz16: This field is the FAT12/FAT16 16-bit count of sectors occupied by ONE FAT.
  fseek(file, 22, SEEK_SET); // Offset: 22 bytes, Size: 2 bytes
  fread(&BPB_FATSz16, 1, 2, file);
  //printf("%s %d\n", "BPB_FATSz16", BPB_FATSz16);

  // BPB_TotSec16: This field is the old 16-bit total count of sectors on the volume.
  fseek(file, 19, SEEK_SET); // Offset: 19 bytes, Size: 2 bytes
  fread(&BPB_TotSec16, 1, 2, file);
  //printf("%s %d\n", "BPB_TotSec16", BPB_TotSec16);

  // BPB_NumFATs: The count of FAT data structures on the volume.
  fseek(file, 16, SEEK_SET); // Offset: 16 bytes, Size: 1 byte
  fread(&BPB_NumFATs, 1, 1, file);
  //printf("%s %d\n", "BPB_NumFATs", BPB_NumFATs);

  // RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
  // BPB_RootEntCnt: For FAT12 and FAT16 volumes, this field contains the count of 32-byte directory entries in the root directory.
  fseek(file, 17, SEEK_SET); // Offset 17 bytes, Size: 2 bytes
  fread(&BPB_RootEntCnt, 1, 2, file);
  //printf("%s %d\n", "BPB_RootEntCnt", BPB_RootEntCnt);

  // BPB_BytsPerSec: Count of bytes per sector.
  fseek(file, 11, SEEK_SET); // Offset: 11 bytes, Syze 2 bytes
  fread(&BPB_BytsPerSec, 1, 2, file);
  //printf("%s %d\n", "BPB_BytsPerSec", BPB_BytsPerSec);

  // RootDirSectors: Determine the count of sectors occupied by the root directory
  RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec;
  //printf("%s %d\n", "RootDirSectors ", RootDirSectors);

  // DataSec = TotSec – (BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);
  // BPB_RsvdSecCnt: Number of reserved sectors in the Reserved region of the volume.
  fseek(file, 14, SEEK_SET); // Offset: 14 bytes, Size: 2 byte
  fread(&BPB_RsvdSecCnt, 1, 2, file);
  //printf("%s %d\n", "BPB_RsvdSecCnt", BPB_RsvdSecCnt);

  DataSec = BPB_TotSec16 - (BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz16) + RootDirSectors);
  //printf("%s %d\n", "DataSec ", DataSec);

  // BPB_SecPerClus: Number of sectors per allocation unit.
  fseek(file, 13, SEEK_SET); // Offset: 13 bytes, Size: 1 byte
  fread(&BPB_SecPerClus, 1, 1, file);
  //printf("%s %c\n", "BPB_SecPerClus", BPB_SecPerClus);

  // Un cop tenim el nombre de sectors necessitem determinar el nombre de clusters.
  CountofClusters = DataSec / BPB_SecPerClus;
  //printf("%s %d\n", "CountofClusters", CountofClusters);

  // BS_VolLab: Volume label. This field matches the 11-byte volume label recorded in the root directory.
  fseek(file, 43, SEEK_SET); // Offset: 43 bytes, Size 11 bytes
  fread(infoFAT16.label, 1, 11, file);

  // FirstRootDirSecNum = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz16);
  FirstRootDirSecNum = ((BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz16)) * BPB_BytsPerSec);

  infoFAT16.countofClusters = CountofClusters;
  infoFAT16.BPB_BytsPerSec = BPB_BytsPerSec;
  infoFAT16.size = BPB_BytsPerSec * BPB_SecPerClus;
  infoFAT16.sectorsXCluster = BPB_SecPerClus;
  infoFAT16.numFATs = BPB_NumFATs;
  infoFAT16.maxRootEntries = BPB_RootEntCnt;
  infoFAT16.sectorsXFAT = BPB_TotSec16;
  infoFAT16.reservedSectors = BPB_RsvdSecCnt;
  infoFAT16.BPB_FATSz16 = BPB_FATSz16;
  infoFAT16.FirstRootDirSecNum = FirstRootDirSecNum;

  return infoFAT16;
}
