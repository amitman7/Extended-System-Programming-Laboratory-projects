all: task1

main: task1.o
	gcc -m32 -Wall -g task1.o -o task1

main.o: task1.c
	gcc -m32 -Wall -g -c task1.c

clean:
	rm -f task1.o task1