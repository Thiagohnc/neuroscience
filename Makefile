CC = g++
FLAGS = -Wall -O3 -std=c++17

all: clean main

main: main.cpp graph_generator.o graph.o params.o utils.o
	$(CC) $(FLAGS) main.cpp graph_generator.o graph.o params.o utils.o -o main

graph_generator.o: graph_generator.cpp graph_generator.hpp graph.hpp utils.hpp
	$(CC) $(FLAGS)  graph_generator.cpp -c -o graph_generator.o

graph.o: graph.cpp graph.hpp utils.hpp
	$(CC) $(FLAGS)  graph.cpp -c -o graph.o

params.o: params.cpp params.hpp utils.hpp
	$(CC) $(FLAGS) params.cpp -c -o params.o

utils.o: utils.cpp utils.hpp
	$(CC) $(FLAGS)  utils.cpp -c -o utils.o

clean:
	rm -rf *.o *~ main