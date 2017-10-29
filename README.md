# Parallelized Distance-1 Graph Coloring
This program executes sequential and parallel versions of distance-1 graph coloring algorithm, outputs the execution
times of sequential and parallel versions(with 1, 2, 4, 8, 16 threads) as well as the total number of color used in
each execution and runs a correctness check to confirm no adjacent vertices has the same color.

**Implemented by:**
 * [M.Mucahid Benlioglu](https://github.com/mbenlioglu)


## Getting Started
There are two make files included this project. “Makefile-data” is to crawl the test data that can be used for testing
and developement. (tests for current implementation done on these datasets). Running following command on terminal will
download and extract the data into "./data/"

    $ make -f Makefile-data

Input data is in [MTX format](http://math.nist.gov/MatrixMarket/formats.html) and converted into [CRS format](http://netlib.org/linalg/html_templates/node91.html)
for processing. 

Another Makefile is also included to compile codes into executable in Linux environment (Use visual studio with /openmp flag
for Windows). Executing following command under "./src" will create an executable named "coloring" which takes single input
argument, which is the path to the dataset.

    $ make

The program will run sequential and parallel versions of the algorithm and output the execution time and number of colors used.


## Algorithm & Implementation
In this section, sequential and parallel version of the algorithm will be explained. Performance results of each algorithm can
be found in results section.

### Sequential Algorithm
Since the size of the graphs are huge, the algorithm follows a greedy approach instead of dynamic programming, therefore
total number of colors used in the graph may result in a suboptimal number. The algorithm implements backtracking search
algorithm but instead of traversing all possibilities, in each step it selects the most suitable vertex and color without
turning back to try other possibilities.

Suitability metric for vertices is minimum remaining values (MRV), since we don’t have a limitation for maximum number of
colors, definition for a vertex with MRV changes to a vertex that has the most constraints. As a tie-breaker among MRV
variables, degree heuristic is used, which means the vertex that introduces the most constraints on the remaining vertices
is chosen. Finally, the least constraining color is chosen (i.e. the one that introduces least number of new constraints to
remaining vertices) as the color of the selected vertex.

### Parallel Version
Parallelized implementation uses a divide and conquer approach, meaning that the graph is divided into smaller subgraphs,
which are colored separately and concurrently according to the sequential logic, then these sub graphs are combined to get
the final answer.

Since the subgraphs are colored independently, possible conflicts occur at the borders of the subgraphs, therefore each
combination step has a conflict detection and fixing step to remove these conflicts.

## Results
The following table shows the executions times of implementations explained in the previous sections.

![Performance Result Table](/res/perf_results.png)
