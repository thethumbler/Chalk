all:
	gcc src/chalk.c -o bin/chalk -g -lm
clean:
	rm bin/chalk
install:
	cp bin/chalk /bin/chalk
