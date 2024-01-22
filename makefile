all: shooter
shooter.o: ./SourceFiles/shooter.c
	gcc -c ./SourceFiles/shooter.c
info.o: ./SourceFiles/info.c ./HeaderFiles/info.h
	gcc -c ./SourceFiles/info.c
findOrDelete.o: ./SourceFiles/findOrDelete.c ./HeaderFiles/findOrDelete.h
	gcc -c ./SourceFiles/findOrDelete.c
shooter: shooter.o info.o findOrDelete.o
	gcc shooter.o info.o findOrDelete.o -o shooter -Wall -Wextra

.PHONY: clean
clean:
	rm -rf *.o
