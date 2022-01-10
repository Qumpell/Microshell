microshell: microshell.c
	gcc -Wall -std=c99 -o microshell microshell.c

clean:
	rm -f *.o microshell

