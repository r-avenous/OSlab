CC=g++
FLAGS=-Wall -std=c++14

output:
	$(CC) $(flags) main.c utility.c -o terminal

terminal: history.o utility.o
	$(CC) $(flags) history.o utility.o -lreadline -o terminal

history.o: history.c history.h
	$(CC) $(flags) -c history.c -o history.o

utility.o: utility.c utility.h
	$(CC) $(flags) -c utility.c -o utility.o
