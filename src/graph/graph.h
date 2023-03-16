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



class Graph {

    struct Edge {
        int dest;   // Destination node
        float weight; // An integer weight
    };

    struct Node {
        std::list<Edge> adj; // The std::list of outgoing edges (to adjacent nodes)
        int pred = 0;
        bool visited;   // Flag to indicate if the node has been visited
        float dist;

        int id;
        std::string district;
        std::string county;
        std::string parish;
        std::string address;
        float latitude;
        float longitude;
        int inspection_time;    // in minutes
        float inspection_utility;
        std::vector<int> opening_hours; // std::vector<int>(24, 0/1) 0 = closed, 1 = open
    };

    struct Time {
        int milliseconds;
        int seconds;
        int minutes;
        int hours;
        int days;

        /**
         * Set time
         * @param milliseconds
         * @param seconds
         * @param minutes
         * @param hours
         * @param days
         */
        void setTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0, int days = 0);

        /**
         * Add time
         * @param milliseconds
         * @param seconds
         * @param minutes
         * @param hours
         * @param days
         */
        void addTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0, int days = 0);

        /**
         * Add time
         * @param time
         */
        void addTime(Time time);

        /**
         * Subtract time
         * @param milliseconds
         * @param seconds
         * @param minutes
         * @param hours
         * @param days
         */
        void subTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0, int days = 0);

        /**
         * Subtract time
         * @param time
         */
        void subTime(Time time);

        /**
         * Compare two times
         * @param rhs
         * @return
         */
        bool operator<(const Time &rhs) const;

        /**
         * Forward time to next hour
         */
        void toNextHour();

        /**
         * Roll back time to previous hour
         */
        void toPreviousHour();

        /**
         * Convert time to seconds
         * @return
         */
        float toSeconds();
    };

    int n;              // Graph size (vertices are numbered from 1 to n)
    bool hasDir;        // false: undirected; true: directed
    std::vector<Node> nodes; // The std::list of nodes being represented

    int nrVehicles;    // Number of vehicles

    Time departure_time;    // Time when vehicles start working
    Time max_work_time; // Maximum time vehicles can work
    Time limit_time;    // Time when vehicles stop working
    std::vector<Time> times;     // Current times for each vehicle

    std::mt19937 engine;

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
     * Get Nodes
     * @return  Nodes
     */
    std::map<int, std::pair<float, float>> getNodes();

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
     * Evaluate Solution
     * @param solution  Solution
     * @return  Solution Value
     */
    int evaluate_solution(const std::vector<std::list<int>>& solution);

    /**
     * Get Random Node different from idx
     * @param idx   Index of Establishment
     * @return  Random Node
     */
    int random_node(int idx) const;

    /**
     * Get Closest Node different from idx and incompatible
     * @param idx
     * @return
     */
    int closest_node(int idx, std::list<int> incompatible) const;

    /**
     * Get order (th) Closest Node different from idx
     * @param idx
     * @return
     */
    int closest_node(int idx, int order=1) const;

    /**
     * Get Random Solution
     * @return  Random Solution
     */
    std::vector<std::list<int>> generate_random_solution(bool log=false);

    /**
     * Get Solution with closest nodes
     * @return  Random Solution
     */
    std::vector<std::list<int>> generate_closest_solution(bool log=false);

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
    void printDetailedSolution(const std::vector<std::list<int>>& solution, bool log=false);

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

    std::vector<std::vector<std::list<int>>> getNeighbours(std::vector<std::list<int>> solution, int neighborhood_size, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&));

    std::vector<std::list<int>> hillClimbing(int iteration_number, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    std::vector<std::list<int>> simulatedAnnealing(int iteration_number, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    std::vector<std::list<int>> tabuSearch(int iteration_number, int tabu_size, int neighborhood_size, std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    std::vector<std::list<int>> geneticAlgorithm(int iteration_number, int population_size, int tournament_size, int mutation_probability, std::vector<std::vector<std::list<int>>> (Graph::*crossover_func)(const std::vector<std::list<int>>&, const std::vector<std::list<int>>&), std::vector<std::list<int>> (Graph::*mutation_func)(const std::vector<std::list<int>>&), int (Graph::*evaluation_func)(const std::vector<std::list<int>>&), bool log=false);

    bool check_solution(std::vector<std::list<int>> vector1);

    void plotGraph();

    std::vector<std::vector<std::list<int>>> generatePopulation(int population_size);

    std::vector<std::list<int>> tournamentSelection(std::vector<std::vector<std::list<int>>> population, int size, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    std::vector<std::list<int>> rouletteSelection(std::vector<std::vector<std::list<int>>> population, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    std::vector<std::vector<std::list<int>>> replace_least_fittest(std::vector<std::vector<std::list<int>>> population, std::vector<std::list<int>> new_solution, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    std::pair<std::vector<std::list<int>>, int> get_greatest_fit(std::vector<std::vector<std::list<int>>> population, int (Graph::*evalFunction)(const std::vector<std::list<int>>&));

    std::vector<std::vector<std::list<int>>> crossover_test(const std::vector<std::list<int>>& parent1, const std::vector<std::list<int>>& parent2);
};

#endif /* GRAPH_H_ */
