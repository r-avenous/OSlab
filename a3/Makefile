CC=g++
flags=-Wall -std=c++11

graph:	main.cpp producer.cpp consumer.cpp
	$(CC) $(flags) main.cpp -o main
	$(CC) $(flags) producer.cpp -o producer
	$(CC) $(flags) consumer.cpp -o consumer

runop:  main.cpp producer.cpp consumer.cpp
	$(CC) $(flags) main.cpp -o main
	$(CC) $(flags) producer.cpp -o producer
	$(CC) $(flags) consumerOP.cpp -o consumer
	./main -optimize

clean:
	ls | grep -P "^[0-9]\.txt$" | xargs -d"\n" rm -f
	rm -f main producer consumer 
