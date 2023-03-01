CC=g++
flags=-Wall -std=c++11

sns:	sns.cpp musae_git_edges.csv
	$(CC) $(flags) sns.cpp -o sns

run:	sns
	./sns

clean:
	rm -f sns

debug:	sns.cpp musae_git_edges.csv
	$(CC) $(flags) -g sns.cpp -o sns
	gdb sns