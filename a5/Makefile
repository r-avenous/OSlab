hotel:	main.o
	g++ -o hotel main.o cleaner.o guest.o -lpthread

main.o: main.cpp guest.o cleaner.o
	g++ -c main.cpp

guest.o: guest.cpp utils.hpp guest.hpp
	g++ -c guest.cpp

cleaner.o: cleaner.cpp utils.hpp cleaner.hpp
	g++ -c cleaner.cpp

clean:
	rm *.o hotel