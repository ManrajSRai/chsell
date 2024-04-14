all: cshell

clean:
	rm -f cshell

cshell: cshell.c
	gcc -o cshell cshell.c
