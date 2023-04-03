#ifndef GRAPH_H_
#define GRAPH_H_

#include <matplot/matplot.h>
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include <climits>
#include <cmath>
#include <queue>
#include <cmath>
#include <random>
#include <utility>
#include <set>
#include "Time.h"

class Graph {

    struct Edge {
        int dest;   // Destination node
        float weight; // An integer weight
    };

    struct Node {
        std::list<Edge> adj; // The std::list of outgoing edges (to adjacent nodes)
        int pred = 0;
        bool visited;   // Flag to indicate if the node has been visited

        int id;
        std::string district;
        std::string county;
        std::string parish;
        std::string address;

        float latitude;
        float longitude;

        int inspection_time;    // in minutes
        int inspection_utility;
        std::vector<int> opening_hours; // std::vector<int>(24, 0/1) 0 = closed, 1 = open

        float getLatitude() const;
        float getLongitude() const;
        const std::string &getAddress() const;
    };

    int n;              // Graph size (vertices are numbered from 1 to n)
    bool hasDir;        // false: undirected; true: directed
    std::vector<Node> nodes; // The std::list of nodes being represented

    int nrVehicles{};    // Number of vehicles

    Time departure_time{};    // Time when vehicles start working
    Time max_work_time{}; // Maximum time vehicles can work
    Time limit_time{};    // Time when vehicles stop working
    std::vector<Time> times;     // Current times for each vehicle

    std::mt19937 engine;

    std::vector<int> iterations;
    int iterations_optimal{};
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    std::chrono::time_point<std::chrono::system_clock> optimal_time;


private:

    /**
     * Set number of vehicles
     * @param n
     */
    void setNrVehicles(int n);

public:
    /**
     * Graph Constructor
     * @param nodes Number of establishments
     * @param dir   Graph is directed or not
     * @param departure_time    Time when vehicles start working
     * @param max_work_time     Maximum time vehicles can work
     */
    explicit Graph(int nodes, bool dir = false, Time departure_time = {0, 0, 0, 0}, Time max_work_time = {0, 0, 0, 0});

   /**
    * Add Edge to Graph
    * @param src    Source Node
    * @param dest   Destination Node
    * @param weight Weight of Edge
    */
    void addEdge(int src, int dest, float weight = 1.00);

    /**
     * Set Node
     * @param index    Index of Establishment
     * @param address   Address of Establishment
     * @param latitude  Latitude of Establishment
     * @param longitude     Longitude of Establishment
     * @param inspection_utility    Inspection Utility of Establishment
     * @param inspection_time   Inspection Time of Establishment
     * @param opening_hours     Opening Hours of Establishment
     */
    void setNode(int index, std::string district, std::string county, std::string parish, std::string address, float latitude, float longitude, float inspection_utility, int inspection_time, std::vector<int> opening_hours);

    /**
     * Get Node
     * @param at    Id of establishment
     * @return  Node
     */
    Node getNode(int at);

    /**
     * Get Path from origin to destination
     * @param origin    Origin Establishment
     * @param destin    Destination Establishment
     * @return  Path
     */
    std::list<int> path(int origin, int destin);

    /**
     * Maximize number of establishments
     * @param solution  Solution
     * @return  Solution Value
     */
    int evaluate_solution_1(const std::vector<std::list<int>>& solution);

    /**
     * Minimize parish transfers
     * @param solution  Solution
     * @return  Solution Value
     */
    int evaluate_solution_2(const std::vector<std::list<int>>& solution);

    /**
     * Get Random Solution
     * @return  Random Solution
     */
    std::vector<std::list<int>> generate_random_solution(bool log=false);

    /**
     * Get Solution with closest nodes
     * @return  Solution
     */
    std::vector<std::list<int>> generate_closest_solution(bool log=false);

    /**
     * Get Solution using a* algorithm
     * @return  Solution
     */
    std::vector<std::list<int>> generate_a_star_solution(bool log=false);

    /**
     * Get Distance between two establishments
     * @param origin    Origin Establishment
     * @param destin    Destination Establishment
     * @return
     */
    float getDistance(int origin, int destin);

    /**
     * Display all establishments addresses
     */
    void showAllEstablishments();

    /**
     * Display solution
     * @param solution
     */
    static void printSolution(const std::vector<std::list<int>>& solution);

    /**
     * Display more details about a solution
     * @param solution
     */
    void printDetailedSolution(const std::vector<std::list<int>>& solution);

    /**
     * Get total travel time of a solution
     * @param solution  Solution
     * @return  Total travel time
     */
    float totalTravelTime(const std::vector<std::list<int>>& solution, bool log=false);

    /**
     * Get total waiting time of a solution
     * @param solution  Solution
     * @return  Total waiting time
     */
    float totalWaitingTime(const std::vector<std::list<int>>& solution, bool log=false);

    /**
     * Get total operation time of a solution
     * @param solution  Solution
     * @return  Total operation time
     */
    float totalOperationTime(const std::vector<std::list<int>>& solution, bool log=false);

    /**
     * Get minimum time required to be able to do dislocation
     * @param origin    Origin Establishment
     * @param destin    Destination Establishment
     * @param time  Current time
     * @return  Minimum time required to be able to do dislocation
     */
    Time minimumOperationTime(int origin, int destin, Time time, bool log=false);

    /**
     * Get time of dislocation
     * @param origin    Origin Establishment
     * @param destin    Destination Establishment
     * @param time  Current time
     * @return  Time of dislocation
     */
    Time operationTime(int origin, int destin, Time time, bool log=false);

    /**
     * Exchange two establishments in a solution
     * @param solution
     * @return
     */
    std::vector<std::list<int>> mutation_solution_1(const std::vector<std::list<int>>& solution);

    /**
     * Exchange one establishment for one that didn't appear in the solution
     * @param solution
     * @return
     */
    std::vector<std::list<int>> mutation_solution_2(const std::vector<std::list<int>>& solution);

    /**
      * Generate a new path between two nodes
      * @param solution
      * @return
      */
    std::vector<std::list<int>> mutation_solution_3(const std::vector<std::list<int>>& solution);

    /**
      * Scramble the order of a subset of nodes in a vehicle's path
      * @param solution
      * @return
      */
    std::vector<std::list<int>> mutation_solution_4(const std::vector<std::list<int>>& solution);

    /**
      * Remove a node from a vehicle's path and insert two new nodes
      * @param solution
      * @return
      */
    std::vector<std::list<int>> mutation_solution_5(const std::vector<std::list<int>>& solution);

    /**
     * Choose a random mutation
     * @param solution
     * @return
     */
    std::vector<std::list<int>> mutation_solution_6(const std::vector<std::list<int>>& solution);

    /**
     * Fill given solution with more establishments in vans
     * @param child
     */
    std::vector<std::list<int>> fillSolution(const std::vector<std::list<int>> &child);

    /**
     * Crossover two solutions (father and mother -> child1(father[0:mid], mother[mid:end]) and child2(mother[0:mid], father[mid:end]))
     * @param father_solution
     * @param mother_solution
     * @return
     */
    std::pair<std::vector<std::list<int>>, std::vector<std::list<int>>> crossover_solutions_1(const std::vector<std::list<int>> &father_solution, const std::vector<std::list<int>> &mother_solution);

    /**
     * Crossover two solutions (father and mother -> child1(father[0:mid1], mother[mid1:mid2], father[mid2:end) and child2(mother[0:mid1], father[mid1:mid2], mother[mid2:end)))
     * @param father_solution
     * @param mother_solution
     * @return
     */
    std::pair<std::vector<std::list<int>>, std::vector<std::list<int>>> crossover_solutions_2(const std::vector<std::list<int>> &father_solution, const std::vector<std::list<int>> &mother_solution);

    /**
     * Crossover two solutions (father and mother -> child1(best 5% father and 10% mother) and child2(best 5% mother and 10% father))
     * @param father_solution
     * @param mother_solution
     * @return
     */
    std::pair<std::vector<std::list<int>>, std::vector<std::list<int>>> crossover_solutions_3(const std::vector<std::list<int>> &father_solution, const std::vector<std::list<int>> &mother_solution);

    /**
     * Get neighbours of a solution
     * @param solution
     * @param neighborhood_size
     * @param mutation_func
     * @return
     */
    std::vector<std::vector<std::list<int>>> getNeighbours(const std::vector<std::list<int>>& solution, int neighborhood_size, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&));

    /**
     * Hill climbing algorithm
     * @param iteration_number
     * @param mutation_func
     * @param evaluation_func
     * @param log
     * @return
     */
    std::vector<std::list<int>> hillClimbing(int iteration_number, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    /**
     * Simulated annealing algorithm
     * @param iteration_number
     * @param mutation_func
     * @param evaluation_func
     * @param log
     * @return
     */
    std::vector<std::list<int>> simulatedAnnealing(int iteration_number, float coldness_ratio, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    /**
     * Tabu search algorithm
     * @param iteration_number
     * @param tabu_size
     * @param neighborhood_size
     * @param mutation_func
     * @param evaluation_func
     * @param log
     * @return
     */
    std::vector<std::list<int>> tabuSearch(int iteration_number, int tabu_size, int neighborhood_size, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    /**
     * Check if a queue contains a given element
     * @param queue
     * @param element
     * @return
     */
    bool queueContainsElem(std::queue<int> queue, int element);


    /**
     * Genetic algorithm
     * @param iteration_number
     * @param population_size
     * @param tournament_size
     * @param mutation_probability
     * @param crossover_func
     * @param mutation_func
     * @param evaluation_func
     * @param log
     * @return
     */
    std::vector<std::list<int>> geneticAlgorithm(int iteration_number, int population_size, int tournament_size, int mutation_probability, std::pair<std::vector<std::list<int>>, std::vector<std::list<int>>> (Graph::*crossover_func)(const std::vector<std::list<int>>&, const std::vector<std::list<int>>&), std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    /**
     * Check if a solution is valid
     * @param solution
     * @return
     */
    bool check_solution(std::vector<std::list<int>> solution);

    /**
     * Plot graph of initial solution
     */
    void plot_initial_solution(std::vector<std::list<int>> solution);

    /**
     * Generate a random population
     * @param population_size
     * @return
     */
    std::vector<std::vector<std::list<int>>> generatePopulation(int population_size);

    /**
     * Tournament selection
     * @param population
     * @param size
     * @param evalFunction
     * @return
     */
    std::vector<std::list<int>> tournamentSelection(std::vector<std::vector<std::list<int>>> population, int size, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    /**
     * Roulette selection
     * @param population
     * @param evalFunction
     * @return
     */
    std::vector<std::list<int>> rouletteSelection(std::vector<std::vector<std::list<int>>> population, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    /**
     * Replace least fittest solution
     * @param population
     * @param new_solution
     * @param evalFunction
     * @return
     */
    std::vector<std::vector<std::list<int>>> replace_least_fittest(std::vector<std::vector<std::list<int>>> population, std::vector<std::list<int>> new_solution, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    /**
     * Get greatest fit solution
     * @param population
     * @param evalFunction
     * @return
     */
    std::pair<std::vector<std::list<int>>, int> get_greatest_fit(std::vector<std::vector<std::list<int>>> population, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    /**
     * Plot evolution graph
     * @param iterations
     * @param title
     */
    void evolutionGraph(std::vector<int> iterations, std::string title);

    /**
     * Compare algorithms
     * @param sol1
     * @param sol2
     * @param sol3
     * @param sol4
     */
    void compare_algorithms(std::vector<int> sol1, std::vector<int> sol2,
                            std::vector<int> sol3, std::vector<int> sol4, int iterations);

    /**
     * Get iterations
     * @return
     */
    const std::vector<int> &getIterations() const;

    /**
     * Get number of iteration, where was found the last optimal solution
     * @return
     */
    int getIterationsOptimal();

    /**
     * Get runtime
     * @return
     */
    float getRuntime();

    /**
     * Get runtime of the last optimal solution
     * @return
     */
    float getRuntimeOptimal();

    /**
     * Start timer
     */
    void startTimer();

    /**
     * End timer
     */
    void endTimer();

    /**
     * Update timer
     * @param iteration of newer best solution
     */
    void updateTimer(int i);
};

#endif /* GRAPH_H_ */
