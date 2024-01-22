/***********************************************
*
* Pràctica 'The Shooter'
* Sistemes Operatius Avançats, Curs 2021-22
*
* @author Oscar Julian Ponte (oscar.julian)
*
* findOrDelete.c
* @Proposit: Localitzar i/o eliminar el fitxer sol·licitat dins del volum
************************************************/

//Llibreries propies
#include "../HeaderFiles/findOrDelete.h"

/*
    PROCEDIMENTS & FUNCIONS
*/

/***********************************************
*
* @Finalitat: Retornar l'inode del fitxer pertinent
* @Parametres: in: file = FD del volum
                   nInode = Numero del inode
                   InfoInodeEXT2 = Estructura informació de l'inode
                   InfoBlockEXT2 = Estructura informació del block
                   InfoVolumeEXT2 = Estructura informació del volum
                   delete: bool si elimina o no.
* @Retorn:     out: inode: retornem l'inode
*
************************************************/
InodeInfo getInode(FILE *file, int nInode, InfoInodeEXT2 infoInodeEXT2, InfoBlockEXT2 infoBlockEXT2, bool delete) {
  InodeInfo inode;
  BlockGroupDescriptor blockGroupDescriptor;

  /* Block Group Descriptor Table: is an array of block group descriptor, used to define parameters of all the block groups. It provides the location
  of the inode bitmap and inode table, block bitmap, number of free blocks and inodes, and some other useful information. */
  fseek(file, (infoBlockEXT2.firstBlock + 1) * (infoBlockEXT2.size + 1024), SEEK_SET);
  fread(&blockGroupDescriptor, 1, sizeof(BlockGroupDescriptor), file);

  // Per a trobar un inode prmier hem de trobar el block group:
  int blockGroup = (nInode - 1) / infoInodeEXT2.inodesGroup;

  // Un cop tenim el block, busquem el index de la taula de inodes:
  int localInodeIndex = (nInode - 1) % infoInodeEXT2.inodesGroup;

  // Un cop tenim el index de la taula de inodes ja podem buscar la posició del inode (en la taula).
  int inodePos = localInodeIndex * infoInodeEXT2.size;

  // Un cop tenim la posició del inode respecter la taula, mirem la posicio de la taula
  int tablePos = blockGroupDescriptor.bg_inode_table + (blockGroup * infoBlockEXT2.groupBlocks);

  // Amb tot això, ja podem llegir el inode.
  fseek(file, tablePos * (infoBlockEXT2.size + 1024) + inodePos, SEEK_SET); // Offset: 32 bytes, Size: 4 bytes
  fread(&inode, 1, sizeof(InodeInfo), file);

  // En el cas del delete, editarem els camps necessaris per 'buidar' el inode
  if (delete) {

    // Netejem la taula de i_block (adreces de la informació de l'inode)
    for (int p = 0; p < 15; p++) {
      inode.i_block[p] = 0;
    }

    inode.i_size = 0; // Tamany a 0.
    inode.i_dtime = time(0); // value representing the number of seconds since january 1st 1970, of when the inode was deleted.
    inode.i_flags = 0x01; // 0x01 = secure deletion

    // Un cop hem 'buidat' les dades de l'inode, sobreescriurem les daes.
    fseek(file, (tablePos * (infoBlockEXT2.size + 1024)) + inodePos, SEEK_SET); // Offset: 32 bytes, Size: 4 bytes
    fwrite(&inode, 1, sizeof(InodeInfo), file);
  }

  //printf("TOTAL: %d\n", tablePos * (infoBlockEXT2.size + 1024) + inodePos);

  return inode;
}

/***********************************************
*
* @Finalitat: Buscar o eliminar fitxer en el volum EXT2 proporcionat
* @Parametres: in: fileName = nom del fitxer a buscar
                   file = FD del volum
                   nInode = Numero del inode
                   delete: bool si elimina o no.
* @Retorn:    out: size: tamany del fitcher trobat
*
************************************************/
int findOrDeleteEXT2(char *fileName, FILE *file, int nInode, bool delete) {
  InfoInodeEXT2 infoInodeEXT2 = readInodeEXT2(file);
  InfoBlockEXT2 infoBlockEXT2 = readBlockEXT2(file);
  /* En aquest apartat ens centrarem en els inodes i el block group descriptor table.
  El primer que farem serà llegir un 'Inode: Each object in the filesystem is represented by an inode.'' */
  InodeInfo inode = getInode(file, nInode, infoInodeEXT2, infoBlockEXT2, false);

  DIR_StructEXT2 dir_StructEXT2;
  DIR_StructEXT2 dir_Struct_EDIT;

  bool final = false;
  int pos = 0;
  int posAnterior = 0;
  int size = -1;
  char DIR_Name[100];
  int InSize = infoBlockEXT2.size + 1024;

  for (int i = 0; pos < inode.i_size ; i++) {
    final = false;
    // Directories are stored as data block and referenced by an inode.
    // Ens col·loquem a la posició per llegir directoris.

    fseek(file, (Offs_SB + (InSize * (inode.i_block[i] - 1)) + pos), SEEK_SET);

    while(!final) {
    //while(dir_StructEXT2.rec_len || pos > inode.i_size) {

      // Llegirem el directori de la posició actual (A directory file is a linked list of directory entry structures).
      fread(&dir_StructEXT2, 1, sizeof(DIR_StructEXT2), file);

      // Ja tenim la informació pertinent a la estructura actual, agafarem el nom.
      // dir_StructEXT2.name --> Offset 8, Size 0-255 (dir_StructEXT2.name_len)
      fread(&DIR_Name, 1, dir_StructEXT2.name_len, file);
      DIR_Name[(int)dir_StructEXT2.name_len] = '\0';

      // Un cop hem llegit el nom, augmentem el punter de la posició que llegim.
      pos = pos + dir_StructEXT2.rec_len;

      // Comprobarem si hem arribat al final, bé del inode o de la taula de inodes.
      if ((dir_StructEXT2.rec_len == 0) || (pos > inode.i_size)) {
        final = true;
      }

      // El primer que farem amb aquesta estructura es mirar si el tipus es de 'file' o de 'directory'
      // file_type: Value 2 = Directory || Value 1 = File
      if (dir_StructEXT2.file_type == 1) {
        //printf("FileName: %s, %s\n", fileName, DIR_Name);

          // Un cop ens hem assegurat de que es tracta d'un fitxer, mirem si es el dessitjat.
          if (!strcasecmp(fileName, DIR_Name)) {
            // Si estem en la funcionalitat de borrar farem el següent:
            if (delete) {
              // 1. Canviarem el camp de 'desplaçament fins al següent directori' (REC_LEN) del directori anterior.
              fseek(file, (Offs_SB + (InSize * (inode.i_block[i] - 1)) + pos - dir_StructEXT2.rec_len - posAnterior), SEEK_SET);
              fread(&dir_Struct_EDIT, 1, sizeof(DIR_StructEXT2), file); // Agafem la informació del directory anterior
              dir_Struct_EDIT.rec_len = dir_Struct_EDIT.rec_len + dir_StructEXT2.rec_len;
              fwrite(&dir_Struct_EDIT, 1, sizeof(dir_StructEXT2), file);

              // 2. Un cop hem utilitzat la informació de l'inode a borrar, podem editar las seves dades.
              inode = getInode(file, dir_StructEXT2.inode, infoInodeEXT2, infoBlockEXT2, true);

              // 3. També hem de modificar el fitxer de la estructura del directory
              dir_StructEXT2.inode = 0; // A value of 0 indicate that the entry is not used.
              fseek(file, (Offs_SB + (InSize * (inode.i_block[i] - 1)) + pos - dir_StructEXT2.rec_len), SEEK_SET);
              fwrite(&dir_StructEXT2, 1, sizeof(dir_StructEXT2), file);

              // 4. Finalment queda canviar la info del superblock (tindrem un free inode més)
              fseek(file, Offs_EXT + 16, SEEK_SET); //Offset: 16 bytes, Size: 4 bytes
              fwrite(&infoInodeEXT2.freeInodes+1, 1, 4, file);
              size = 1;
              final = true;
            } else {
              // Si hem trobat el fitxer, agafarem el inode assossiat i en treurem el SIZE.
              InodeInfo inode = getInode(file, dir_StructEXT2.inode, infoInodeEXT2, infoBlockEXT2, false);
              size = inode.i_size;
              pos = inode.i_size+5;
              final = true;
            }
          }
      } else if ((dir_StructEXT2.file_type == 2) && (strcmp(DIR_Name, ".")) && (strcmp(DIR_Name, "..")) && (strcmp(DIR_Name, "lost+found"))) {
            //printf("CARPETA: %s\n", DIR_Name);

            size = findOrDeleteEXT2(fileName, file, dir_StructEXT2.inode, delete);
            if (size >= 0) {
              // Si hem trobat un size voldrà dir que hem trobat el fitxer corresponent.
              pos = inode.i_size+5;
              final = true;
            }
        }
        fseek(file, (Offs_SB + (InSize * (inode.i_block[i] - 1)) + pos), SEEK_SET);
      }
      posAnterior = dir_StructEXT2.rec_len;

      if (final) {
        break;
      }

    }
  return size;
}


/***********************************************
*
* @Finalitat: Buscar o borrar fitxer en el volum FAT16 proporcionat
* @Parametres: in: fileName = nom del fitxer a buscar
                   file = FD del volum
                   RootClus: arrel del directory
                   delete: bool si elimina o no.
* @Retorn:     out: size: tamany del fitxer trobat
*
************************************************/
int findOrDeleteFAT16(char *fileName, FILE *file, unsigned short RootClus, bool delete) {
  InfoFAT16 infoFAT16 = readInfoFAT16(file);
  bool final = false;
  char DIR_Struct[32];
  char file_name[30];
  char file_ext[4];
  char DIR_Name[8];
  char DIR_Ext[3];
  int DIR_FileSize = -1;

  // Primerament volem recorrer el directori de l'arrel.
  // FirstRootDirSecNum = BPB_ResvdSecCnt + (BPB_NumFATs * BPB_FATSz16);
  int FirstRootDirSecNum = infoFAT16.FirstRootDirSecNum;

  // Si la funció s'ha cridat recursivament, canviarem el FirstRoot per recorrer l'interior de la carpeta que hem trobat.
  if (RootClus != 2) {
    // FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
    // RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
    FirstRootDirSecNum = ((RootClus-2) * infoFAT16.sectorsXCluster * infoFAT16.BPB_BytsPerSec) + FirstRootDirSecNum;
    FirstRootDirSecNum = FirstRootDirSecNum + (infoFAT16.maxRootEntries*32);
  }

  // Un cop ens situem en el First Root Entry, ho recorrem i comprovem si es tracta d'una carpeta o fitxer
  // Si es crida la funció recursivament per que trobem una carpeta, canviarem la posició on buscarem els fitxers.

  fseek(file, FirstRootDirSecNum, SEEK_SET);

  // Recorrem fins a acabar de recórrer tots els fitxers d'una carpeta o fins a acabar el maxRootEntries del nostre volum.
  for (int i = 0; i < infoFAT16.maxRootEntries && !final; i++) {

    // En cada RootEntry trobarem un bloc de 32 Bytes que conté tota la informació de la carpeta.
    fread(&DIR_Struct, 1, 32, file); // Guardem aquesta informació en un array.

    // DIR_Name[11]: 8 chars name + 3 chars extension.
    for (int n = 0; n < 7; n++) {
        DIR_Name[n] = DIR_Struct[n];
      if (DIR_Struct[n] == ' ') {
       DIR_Name[n] = '\0';
      }
    }

    for (int x = 0; x < 3; x++) {
      DIR_Ext[x] = DIR_Struct[x+8];
      if (DIR_Struct[x+8] == ' ') {
        DIR_Ext[x] = '\0';
      }
    }

    // Si aquesta carpeta no conté res, haurem arribat al final.
    // If DIR_Name[0] == 0x00, then the directory entry is free and there are no allocated directory entries after this one.
    if (DIR_Struct[0] == 0x00) {
      final = true;

    // Si tenim contingut comprobem de que es tracta.
    } else {
      // Comprobarem l'atribut de la carpeta per a saber si es tracta de un fitxer o una carpeta.
      // DIR_Attr: specifies attributes of the file (ATTR_DIRECTORY 0x10, ATTR_ARCHIVE 0x20)
      if (DIR_Struct[11] == 0x20) {
          // Ens guaredem el nom del fitxer i la extensió del fitxer demandat.
          int p = 0;
          int j = 0;
          while(fileName[p] != '.' || p == strlen(fileName)) {
            file_name[p] = fileName[p];
            p++;
          }
          file_name[p] = '\0';
          p++;

          while(p < strlen(fileName)) {
            file_ext[j] = fileName[p];
            j++;
            p++;
          }
          file_ext[j] = '\0';

          //printf("FILENAME: %s.%s\n", DIR_Name, DIR_Ext);

          // Un cop tenim totes les dades, comprovarem el fitxer que estem llegint coincideix amb el demandat (tant nom com extensió)
          if ((!strcasecmp(file_name, DIR_Name)) && (!strcasecmp(file_ext, DIR_Ext))) {

            // Si estem en la funcionalitat de borrar farem el següent:
            if (delete) {
              // If DIR_Name[0] == 0xE5, then the directory entry is free (there is no file or directory name in this entry).
              fseek(file, FirstRootDirSecNum + (i*32), SEEK_SET);
              char free = 0xE5;
              fwrite(&free, 1, 1, file);
            }

            // Si coincideix ens guardem el seu SIZE i ja podem acabar la execució.

            // DIR_FileSize: OFFSET: 28, BYTES: 4. Holding this file’s size in bytes.
            // Ajuntem els 4 bytes que tenim en el array en un mateix int.
            DIR_FileSize = (DIR_Struct[28] | ((int)DIR_Struct[29] << 8) | ((int)DIR_Struct[30] << 16) | ((int)DIR_Struct[31] << 8));
            final = true;
            break;
          }
  // En el cas que ens trobem una carpeta, haurem de recorrer el seu interior.
  } else if ((DIR_Struct[11] == 0x10) && ((strcmp(DIR_Name, ".") != 0)) && ((strcmp(DIR_Name, "..") != 0))) {
        // BPB_RootClus: This is set to the cluster number of the first cluster of the root directory
        // DIR_FstClusLO: Offset: 26, Bytes: 2 = Low word of this entry’s first cluster number.
        unsigned short RootClusDir = ((unsigned short)DIR_Struct[27] << 8) | (unsigned char)DIR_Struct[26];

        // Si ens trobem una carpeta cridarem recursivament la funció per buscar arxius dins d'aquesta.
        DIR_FileSize = findOrDeleteFAT16(fileName, file, RootClusDir, delete);
        if (DIR_FileSize >= 0) {
          // Si hem trobat un size voldrà dir que hem trobat el fitxer corresponent.
          final = true;
        }
        fseek(file, FirstRootDirSecNum + (i*32), SEEK_SET);
      }
    }
  }
  return DIR_FileSize;
}
