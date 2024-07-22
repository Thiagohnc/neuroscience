CC = g++
FLAGS = -Wall -O3 -std=c++17 -I ${BOOST_ROOT} -lz

all: clean main

main: main.cpp graph_generator.o graph.o analysis.o params.o utils.o
	$(CC) $(FLAGS) main.cpp graph_generator.o graph.o analysis.o params.o utils.o -o main

graph_generator.o: graph_generator.cpp graph_generator.hpp graph.hpp utils.hpp
	$(CC) $(FLAGS) graph_generator.cpp -c -o graph_generator.o

graph.o: graph.cpp graph.hpp utils.hpp
	$(CC) $(FLAGS) graph.cpp -c -o graph.o

analysis.o: analysis.cpp analysis.hpp utils.hpp
	$(CC) $(FLAGS) analysis.cpp -c -o analysis.o

params.o: params.cpp params.hpp utils.hpp
	$(CC) $(FLAGS) params.cpp -c -o params.o

utils.o: utils.cpp utils.hpp
	$(CC) $(FLAGS) utils.cpp -c -o utils.o

#gzip.o: external/gzip.cpp
#	$(CC) $(FLAGS) external/gzip.cpp -c -o external/gzip.o

#zlib.o: external/zlib.cpp
#	$(CC) $(FLAGS) external/zlib.cpp -c -o external/zlib.o

clean:
	rm -rf *.o *~ main