hotel:	cleaner.o guest.o main.o
	g++ -o hotel main.o cleaner.o guest.o -lpthread

main.o: main.cpp guest.o cleaner.o
	g++ -c main.cpp -lpthread

guest.o: guest.cpp guest.hpp
	g++ -c guest.cpp -lpthread

cleaner.o: cleaner.cpp cleaner.hpp
	g++ -c cleaner.cpp -lpthread

clean:
	rm *.o hotel



