CC=g++
FLAGS=-Wall -std=c++14

output:
	$(CC) $(flags) main.c utility.c -o terminal

terminal: history.o utility.o
	$(CC) $(flags) history.o utility.o -lreadline -o terminal

history.o: history.cpp history.hpp
	$(CC) $(flags) -c history.cpp -o history.o

utility.o: utility.cpp utility.hpp
	$(CC) $(flags) -c utility.cpp -o utility.o
