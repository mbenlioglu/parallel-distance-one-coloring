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

Another Makefile is also included to compile codes into executable in Linux environment (for Windows use visual studio with
/openmp flag). Executing following command in the project root will create an executable named "coloring" which takes single
input argument, which is the path to the dataset.

    $ make

The program will run sequential and parallel versions of the algorithm and output the execution time and number of colors used.


## Algorithm & Implementation
In this section, sequential and parallel version of the algorithm will be explained. Performance results of each algorithm can
be found in results section.

Note that, colors are represented as short integers starting from 0 to 32,767. This is because, we know that number of maximum
colors used in a graph is bounded by number of maximum edges a vertex can have, and since we are using sparse graphs, this number
doesn’t even get close to the limit.

### Direct Approach
#### Sequential Version
Since the size of the graphs are huge, the algorithm follows a greedy approach instead of dynamic programming, therefore total
number of colors used in the graph may result in a suboptimal number. In this approach, all the vertices are iterated over one-by-one
to find appropriate color that satisfies distance-1 coloring conditions. 

On each iteration (i.e. for every vertex), the smallest color that is not used by its neighboring vertices is assigned to the vertex.
Therefore, this implantation favors a first-fit manner for assigned colors.

In order to find the smallest available color, neighbors of the vertex are visited, and a Boolean array is used to store this
information. Then, by iterating over this Boolean smallest unused color can be easily determined. By this approach, we can
determine smallest unused color in O(k) time complexity, where k represents the number of neighbors of the vertex.

#### Parallel version
Parallelized version, uses the same approach as the sequential version, only difference is that multiple vertices are controlled
and assigned at the same time, which introduces a race condition. Therefore, at the end of initial colorization step, a conflict
detection phase occurs, where all graph is checked for conflicted vertices (i.e. distance-1 neighbors with same color) and these
vertices recolored in parallel. Conflict detection phase repeats until there are no conflicts left on the graph.

### Heuristic Approach
#### Sequential Version
This approach uses the idea behind backtracking search to find the best coloring order and color for vertices to minimize number
of colors used. Because of this heuristic approach, a preprocessing step is introduced to the graph, which creates an order among
vertices that will determine which vertex will be colored first.

Suitability metric that will determine the order for vertices is minimum remaining values (MRV), since we don’t have a limitation
for maximum number of colors, definition for a vertex with MRV modified as a vertex that has the most constraints. As a tie-breaker 
mong MRV variables, degree heuristic is used, which means the vertex that introduces the most constraints on the remaining vertices 
s chosen first. 

As color, instead of smallest unused color among neighbors, the least constraining color is chosen (i.e. the one that introduces
least number of new constraints to remaining vertices) for the selected vertex.

#### Parallel Version
Parallelized implementation uses a divide and conquer approach, meaning that the graph is divided into smaller subgraphs, which are
colored separately and concurrently according to the sequential logic, then these sub graphs are combined to get the final answer.

Since the subgraphs are colored independently, possible conflicts occur at the borders of the subgraphs, therefore each combination
step has a conflict detection and fixing step to remove these conflicts.

## Results
In this section, result tables containing various information about execution results is given.

### Direct Approach

![Execution Time Result Table](/res/time_results_direct.png)

**Table 1.1.:** Executions times, speedups and efficiencies of implementations explained in the "Direct Approach" section.

![Color/Conflict fix Table](/res/color_conf_results_direct.png)

**Table 1.2.:** Number of colors used, and number of conflict fix iterations of implementations explained in the "Direct Approach"
section.

### Heuristic Approach

![Execution Time Result Table](/res/time_results_heuristic.png)

**Table 1.1.:** Executions times, speedups and efficiencies of implementations explained in the "Heuristic Approach" section.

![Color/Conflict fix Table](/res/color_conf_results_heuristic.png)

**Table 1.2.:** Number of colors used, and number of conflict fix iterations of implementations explained in the "Heuristic
Approach" section.
