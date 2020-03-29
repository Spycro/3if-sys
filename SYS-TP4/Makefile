
all: main

main: mem.c main.c test.c
	gcc -g -Werror=all -std=gnu99 test.c mem.c main.c -o main

clean:
	rm -f *.o  main
