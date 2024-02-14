all: Loader

Loader: Loader.o start.o
	ld -o Loader Loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

Loader.o: Loader.c
	gcc -m32 -c Loader.c -o Loader.o

start.o: start.s
	nasm -f elf32 start.s -o start.o


clean:
	rm -f Loader.o Loader start.o