output:
	gcc main.c utility.c -o terminal

history: history.o
	gcc history.c utility.c -lncurses -o history 

history.o: history.h
	gcc -c history.c -o history.o
