FLAGS = -g -Wall -o

all: cshell

cshell: cshell.c
	gcc $(FLAGS) cshell cshell.c

clean: 
	rm -rf *.o cshell
