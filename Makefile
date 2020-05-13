satellitedecoder: main.o
	gcc -o satellitedecoder main.o

main.o: main.c
	gcc -c main.c

