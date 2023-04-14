test: goodmalloc.o
	ar rcs libmalloc.a goodmalloc.o
	g++ -Wall -o mergesort mergesort.cpp -I. -L. -lmalloc

run: test
	touch output.txt
	./mergesort > output.txt

goodmalloc.o: goodmalloc.cpp goodmalloc.hpp
	g++ -c goodmalloc.cpp

clean: 
	rm mergesort goodmalloc.o libmalloc.a output.txt