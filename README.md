# TheShooter
App that recognizes, differentiates, and manipulates volumes formatted in different file systems.
Este repositorio contiene la implementación de una aplicación desarrollada en C que permite reconocer, diferenciar y manipular volúmenes formateados en diferentes sistemas de archivos, como FAT16 y EXT2.

## Funcionalidades
### /info
Esta función analiza y extrae información codificada en un volumen específico, diferenciando entre los sistemas de archivos FAT16 y EXT2. Utiliza estructuras y codificaciones apropiadas para cada formato y, a partir de un archivo binario que contiene un volumen, detecta y extrae la información básica pertinente.

### /find
Localiza el archivo solicitado dentro de un volumen y muestra su tamaño en bytes. Inicialmente, la búsqueda se realiza en el directorio raíz del volumen proporcionado. Además, se amplía para realizar búsquedas en profundidad, explorando todas las carpetas del volumen y mostrando el tamaño del archivo si se encuentra.

### /delete
Elimina completamente el archivo solicitado del volumen. Al realizar una nueva búsqueda (/find), la aplicación indicará que el archivo ya no existe.

## Estructura del Proyecto
- shooter.c: Main program that reads commands and executes procedures.
- HeaderFiles: Contains all header files of the project.
- SourceFiles: Contains all source code files of the project.
- Volumes: Contains example volumes for testing functionalities.

## Conclusiones
A nivel personal, la práctica ha sido desafiante pero educativa. La complejidad de manipular información a bajo nivel se hizo evidente, y la necesidad de realizar numerosas comprobaciones y lecturas previas para lograr una funcionalidad aparentemente simple fue un aprendizaje valioso. Aunque no se pudo disfrutar plenamente de los feedbacks de los checkpoints debido a limitaciones de tiempo, el enfoque incremental resultó beneficioso para aplicar gradualmente los conocimientos adquiridos.

## Ejecución:
1. Connect to the 'Montserrat' server at LaSalle.
2. Navigate to the folder where the 'oscar.julian.tar' file has been saved.
3. Unpack the tar file from the same folder using the command: 'tar xvf oscar.julian.tar'
4. 'make clean'
5. 'make'
6. Run the compiled program with the desired operation, specifying the volume and file using the following command: './shooter <operation> <volume_name> <file_name>'

##
- @author: Oscar Julian
- @date: June 2022
