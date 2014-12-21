all:
	gcc -c ../runtime-obj.c
	nasm -f elf64 app.s
	gcc app.o runtime-obj.o
clear:
	rm runtime-obj.o app.o a.out