all:
	gcc -Wall -g -m32 ./units.c -o units
	gcc -Wall -g -m32 ./task1.c -o hexeditplus
	gcc -Wall -g -m32 -fno-pie -fno-stack-protector ./task4.c -o task4

.PHONY: clean
	
clean:
	rm -f ./hexeditplus
	rm -f ./task4
