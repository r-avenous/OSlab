hotel:	main.o utils.o guest.o cleaner.o
	g++ -o hotel main.o cleaner.o guest.o utils.o -lpthread

main.o: main.cpp guest.o cleaner.o
	g++ -c main.cpp

guest.o: guest.cpp utils.hpp guest.hpp
	g++ -c guest.cpp

cleaner.o: cleaner.cpp utils.hpp cleaner.hpp
	g++ -c cleaner.cpp

utils.o: utils.cpp utils.hpp
	g++ -c utils.cpp

clean:
	rm -f *.o hotel