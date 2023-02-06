output:
	gcc main.c utility.c -o terminal

terminal: history.o utility.o
	gcc history.o utility.o -lreadline -o terminal

history.o: history.c history.h
	gcc -c history.c -o history.o

utility.o: utility.c utility.h
	gcc -c utility.c -o utility.o
