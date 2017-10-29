coloring: coloring.cpp
	gcc ./src/graphio.c -c -O3
	gcc ./src/mmio.c -c -O3
	g++ ./src/coloring.cpp -fopenmp -c -O3
	g++ -o coloring coloring.o mmio.o graphio.o -O3 -fopenmp
clean:
	rm coloring *.o
