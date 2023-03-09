CC=g++
flags=-Wall -std=c++11

sns:	sns.cpp musae_git_edges.csv userSimulator.cpp helper.cpp pushUpdate.cpp readPost.cpp
	$(CC) $(flags) sns.cpp helper.cpp userSimulator.cpp pushUpdate.cpp readPost.cpp -o sns -lm -lpthread

run:	sns
	./sns

clean:
	rm -f sns

debug:	sns.cpp musae_git_edges.csv
	$(CC) $(flags) -g sns.cpp -o sns
	gdb sns