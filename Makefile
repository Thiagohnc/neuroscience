CC = g++
FLAGS = -Wall -O3 -std=c++17
EIGEN = /usr/include/eigen3

all: clean main

main: main.cpp graph_generator.o graph.o analysis.o params.o utils.o io.o
	$(CC) $(FLAGS) main.cpp graph_generator.o graph.o analysis.o params.o utils.o io.o -o main -I $(EIGEN)

io.o: io.cpp io.hpp graph.hpp params.hpp utils.hpp
	$(CC) $(FLAGS) io.cpp -c -o io.o

graph_generator.o: graph_generator.cpp graph_generator.hpp graph.hpp utils.hpp
	$(CC) $(FLAGS)  graph_generator.cpp -c -o graph_generator.o

graph.o: graph.cpp graph.hpp utils.hpp
	$(CC) $(FLAGS)  graph.cpp -c -o graph.o

analysis.o: analysis.cpp analysis.hpp utils.hpp
	$(CC) $(FLAGS) analysis.cpp -c -o analysis.o -I $(EIGEN)

params.o: params.cpp params.hpp utils.hpp
	$(CC) $(FLAGS) params.cpp -c -o params.o

utils.o: utils.cpp utils.hpp
	$(CC) $(FLAGS)  utils.cpp -c -o utils.o

clean:
	rm -rf *.o *~ main