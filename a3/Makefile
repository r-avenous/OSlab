CC=g++
flags=-Wall -std=c++11

output:	main.cpp producer.cpp consumerOP.cpp
	$(CC) $(flags) main.cpp -o main
	$(CC) $(flags) producer.cpp -o producer
	$(CC) $(flags) consumerOP.cpp -o consumer

clean:
	ls | grep -P "^[0-9]\.txt$" | xargs -d"\n" rm -f
	rm -f main producer consumer 
