all: myELF

myELF: myELF.o
	gcc -m32 -Wall -g myELF.o -o myELF

myELF.o: myELF.c
	gcc -m32 -c myELF.c -o myELF.o

clean:
	rm -f myELF.o myELF