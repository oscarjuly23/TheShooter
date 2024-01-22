/***********************************************
*
* Pràctica 'The Shooter'
* Sistemes Operatius Avançats, Curs 2021-22
*
* @author Oscar Julian Ponte (oscar.julian)
*
*
* shooter.c
* @Proposit: Main del sistema
************************************************/

//Llibreries del sistema
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

//Llibreries propies
#include "../HeaderFiles/info.h"
#include "../HeaderFiles/findOrDelete.h"

/*
    CONSTANTS
*/

#define ERROR_ARG "Error. Invalid parameter number.\n"
#define ERROR_VOL "Error. Invalid Volume name.\n"
#define ERROR_OP "Error. Invalid operation.\n"
#define ERROR_FAT "Error. Volume not formatted in FAT16 or EXT2."
#define INFO " ------ Filesystem Information ------"
#define I_INODE "INFO INODE"
#define I_BLOCK "INFO BLOCK"
#define I_VOLUME "INFO VOLUME"
#define NOT_FOUND "File not found."

/*
    PROCEDIMENTS & FUNCIONS
*/

/***********************************************
*
* @Finalitat: Mostrar tota la informació EXT2
* @Parametres: in: InfoInodeEXT2 = Estructura informació de l'inode
                   InfoBlockEXT2 = Estructura informació del block
                   InfoVolumeEXT2 = Estructura informació del volum
* @Retorn: -
*
************************************************/
void shooterInfoEXT2(InfoInodeEXT2 infoInodeEXT2, InfoBlockEXT2 infoBlockEXT2, InfoVolumeEXT2 infoVolumeEXT2) {
  /* Informació dels Inodes d'EXT2. Aquest sistema està organitzat en 'n' block groups que contenen la info del volum. */
  printf("\n%s\n", I_INODE);
  printf("Size: %d\n", infoInodeEXT2.size);
  printf("Num Inodes: %d\n", infoInodeEXT2.numInodes);
  printf("First Inode: %d\n", infoInodeEXT2.firstInode);
  printf("Inodes Group: %d\n", infoInodeEXT2.inodesGroup);
  printf("Free Inodes: %d\n", infoInodeEXT2.freeInodes);

  /* Informació del Block d'EXT2. Aquests inodes guarden la informació dels arxius del volum. */
  printf("\n%s\n", I_BLOCK);
  printf("Size Block: %d\n", 1024 + infoBlockEXT2.size);
  printf("Reserved blocks: %d\n", infoBlockEXT2.reservedBlocks);
  printf("Free blocks: %d\n", infoBlockEXT2.freeBlocks);
  printf("Total blocks: %d\n", infoBlockEXT2.totalBlocks);
  printf("First block: %d\n", infoBlockEXT2.firstBlock);
  printf("Group blocks: %d\n", infoBlockEXT2.groupBlocks);

  printf("\n%s\n", I_VOLUME);
  printf("Volume Name: %s\n", infoVolumeEXT2.volumeName);
  printf("Last Checked: %s\n", infoVolumeEXT2.lastChecked);
  printf("Edited last: %s\n", infoVolumeEXT2.editedLast);
  printf("Written last: %s\n", infoVolumeEXT2.writtenLast);
}

/***********************************************
*
* @Finalitat: Mostrar tota la informació FAT16
* @Parametres: in: InfoFAT16 = Estructura amb la informació del volum
* @Retorn: -
*
************************************************/
void shooterInfoFAT16(InfoFAT16 infoFAT16) {
  printf("\nFilesystem: FAT16\n");
  printf("System Name: %s\n", infoFAT16.systemName);
  printf("Size: %d\n", infoFAT16.size);
  printf("Sectors Per Cluster: %d\n", infoFAT16.sectorsXCluster);
  printf("Reserved sectors: %d\n", infoFAT16.reservedSectors);
  printf("Number of FATs: %d\n", infoFAT16.numFATs);
  printf("MaxRootEntries: %d\n", infoFAT16.maxRootEntries);
  printf("Sectors per FAT: %d\n", infoFAT16.sectorsXFAT);
  printf("Label: %s\n", infoFAT16.label);
}

/*
    MAIN
*/

// arg[1] = <operation> - Operació escollida
// arg[2] = <volume_name> - Nom del volum
// arg[3] = <file_name> - Nom del fitxer (Només en /find & /delete)
int main(int argc, char *argv[]) {
  FILE *file; // Punter per al fitxer a llegir (volum)
  unsigned short buffer;

  /********************************************************
   ********************** INFO ****************************
   ********************************************************/

  // Comprobarem l'operació que ha escollit l'usuari.
  if (!strcmp((argv[1]), "/info")) {
    // Comprobem que els arguments introduits són correctes
    if (argc != 3) {
      // Si el nombre d'arguments és incorrecte, indiquem error.
      printf("%s\n", ERROR_ARG);
      return 0;
    }
    if ((file = fopen((argv[2]), "r"))) {
      printf("%s\n", INFO);
      // En l'operació INFO, primer mirarem el tipus de filesystem que té el volum

      // El Superblock del format EXT2 està emmagatzemat en un offset de 1024 bytes des de l'inici, per això li sumem al offset.
      // SEEK_SET: Indiquem que el offset que volem aplicar és des de el principi del fitxer.

      // Segons el filesystem que tinguem (EXT||FAT) agafarem la informació i la mostrarem d'una manera o altre
      // s_magic: Valor de 16bit que identifica que el filesystem és EXT2 (0xEF53)
      fseek(file, Offs_EXT + 56, SEEK_SET);  // Offset: 56 bytes, Size: 2 bytes
      fread(&buffer, 1, 2, file);

      // Si cumpleix aquesta condició sabem que el fylesystem és EXT2, per tant, sino ho compleix comprovarem si es FAT.
      if (buffer == EXT2_FS) {
        InfoInodeEXT2 infoInodeEXT2 = readInodeEXT2(file);
        InfoBlockEXT2 infoBlockEXT2 = readBlockEXT2(file);
        InfoVolumeEXT2 infoVolumeEXT2 = readVolumeEXT2(file);

        shooterInfoEXT2(infoInodeEXT2, infoBlockEXT2, infoVolumeEXT2);
      } else {
        // Per a determinar el tipus de FAT primer hem de determinar el nombre de sectors en la regió d'info del volum:

        // Depenent del nombre de clusters que tinguem ja podem determinar el tipus de FAT que tenim:
        // If(CountofClusters < 4085) {
        // /* Volume is FAT12 */
        // } else if(CountofClusters < 65525) {
        // /* Volume is FAT16 */
        // } else {
        // /* Volume is FAT32 */
        // }

        InfoFAT16 infoFAT16 = readInfoFAT16(file);
        /* Aquest sistema de fitxers està dividit en diferents clusters de la mateixa mida. */
        if ((infoFAT16.countofClusters > 4085) && infoFAT16.countofClusters < 65525) {
          shooterInfoFAT16(infoFAT16);
        } else {
          printf("%s\n", ERROR_FAT);
        }
      }
      fclose(file);

    } else {
      // Si no existeix el volum, indiquem error.
      printf("%s\n", ERROR_VOL);
    }

    /********************************************************
     ********************** FIND ****************************
     ********************************************************/

  // Comprobarem l'operació que ha escollit l'usuari.
  } else if (!strcmp((argv[1]), "/find")) {
    // Comprobem que els arguments introduits són correctes
    if (argc != 4) {
      // Si el nombre de arguments és incorrecte, indiquem error.
      printf("%s\n", ERROR_ARG);

      return 0;
    }
    // En l'operació FIND, primer mirarem el tipus de filesystem que té el volum de la mateixa manera que en el INFO.

    if ((file = fopen((argv[2]), "r"))) {
      int size = -1;
      fseek(file, Offs_EXT + 56, SEEK_SET);
      fread(&buffer, 1, 2, file);

      if (buffer == EXT2_FS) {
        /* En aquest apartat farem la búsqueda del fitxer si el volum es EXT2.
        En aquest sistema de fitxers la informació està estructurada en blocs de dades que estan agrupats. Cada bloc conté el que es diu 'Block Descriptor Table', que guarda informació rellevant.
        Com per exemple el mapa de bits del bloc, mapa de bits de l'inode i també on està l'inici de la taula d'inodes.
        La INODE TABLE és molt important ja que l'inode ens dona la informació bàsica del fitxer i cada fitxer té un inode assosiat.
        El que farem a continuació serà arribar a accedir a aquesta informació llegint els DATA BLOCKS del inode, on trobarem la estructura 'Linked Directory Entry Structure' que conté l'INODE,
        també ens dona més informació com el desplaçament fins els egüent inode, la longitud del nom... */

        size = findOrDeleteEXT2(argv[3], file, 2, false);
        // Si hem trobat una mida de fitxer significa que hem trobat el fitxer demmandat per la comanda introduida.
        if (size >= 0) {
          printf("File found. It is %d bytes in size.\n", size);
        } else {
          printf("%s\n", NOT_FOUND);
        }

      } else {
        InfoFAT16 infoFAT16 = readInfoFAT16(file);

        if ((infoFAT16.countofClusters > 4085) && infoFAT16.countofClusters < 65525) {
          /* En aquest apartat farem la búsqueda del fitxer si el volum es FAT16
          En el primer block trobarem punters cap a las seccions on tindrem els arxius de volum (que hem de buscar). Tenim la taula de FAT dins del FAT Data Structure.
          En el FAT Directory Structure tenim un index de les carpetes i arxius que trobarem dins del Data Area */
          size = findOrDeleteFAT16(argv[3], file, 2, false);
          // Si hem trobat una mida de fitxer significa que hem trobat el fitxer demmandat per la comanda introduida.
          if (size >= 0) {
            printf("File found. It is %d bytes in size.\n", size);

          } else {
            printf("%s\n", NOT_FOUND);
          }

        } else {
          printf("%s\n", ERROR_FAT);
        }
      }

    } else {
      // Si no existeix el volum, indiquem error.
      printf("%s\n", ERROR_VOL);
    }

    /********************************************************
     ********************* DELETE ***************************
     ********************************************************/

  // Comprobarem l'operació que ha escollit l'usuari.
  } else if (!strcmp((argv[1]), "/delete")) {
    // Comprobem que els arguments introduits són correctes
    if (argc != 4) {
      // Si el nombre de arguments és incorrecte, indiquem error.
      printf("%s\n", ERROR_ARG);

      return 0;
    }
    // En l'operació DELETE, primer mirarem el tipus de filesystem que té el volum de la mateixa manera que en el INFO.

    // Parameter 'r+': Opens a file to update both reading and writing. The file must exist.
    if ((file = fopen((argv[2]), "r+"))) {
      int size = -1;
      fseek(file, Offs_EXT + 56, SEEK_SET);
      fread(&buffer, 1, 2, file);

      if (buffer == EXT2_FS) {
        // En aquest apartat farem la eliminació del fitxer si el volum es EXT2

        size = findOrDeleteEXT2(argv[3], file, 2, true);
        // Aquesta funcionalitat funciona de manera semblant al FIND amb la diferencia de que quan trobem l'arxiu l'eliminarem i mostrarem un missatge diferent.
        if (size >= 0) {
          printf("The file %s has been deleted.\n", argv[3]);
        } else {
          printf("%s\n", NOT_FOUND);
        }

      } else {
        InfoFAT16 infoFAT16 = readInfoFAT16(file);

        if ((infoFAT16.countofClusters > 4085) && infoFAT16.countofClusters < 65525) {
          // En aquest apartat farem la eliminació del fitxer si el volum es FAT16
          size = findOrDeleteFAT16(argv[3], file, 2, true);
          if (size >= 0) {
            printf("The file %s has been deleted.\n", argv[3]);

          } else {
            printf("%s\n", NOT_FOUND);
          }

        } else {
          printf("%s\n", ERROR_FAT);
        }
      }

    } else {
      // Si no existeix el volum, indiquem error.
      printf("%s\n", ERROR_VOL);
    }
  } else {
    // Si no existeix la operació, indiquem error.
    printf("%s\n", ERROR_OP);
  }

  return 0;
}
