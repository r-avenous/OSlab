CC=g++
flags=-Wall -std=c++11

graph:	main.cpp producer.cpp consumer.cpp
	$(CC) $(flags) main.cpp -o main
	$(CC) $(flags) producer.cpp -o producer
	$(CC) $(flags) consumer.cpp -o consumer

clean:
	rm -f main producer consumer
