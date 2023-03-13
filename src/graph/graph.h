#ifndef GRAPH_H_
#define GRAPH_H_

#include "minHeap.h"
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include <climits>
#include <cmath>
#include <queue>
#include <cmath>

using namespace std;


class Graph {

    struct Edge {
        int dest;   // Destination node
        float weight; // An integer weight
    };

    struct Node {
        list<Edge> adj; // The list of outgoing edges (to adjacent nodes)
        int pred = 0;
        bool visited;   // Flag to indicate if the node has been visited
        float dist;

        int id;
        string district;
        string county;
        string parish;
        string address;
        float latitude;
        float longitude;
        int inspection_time;    // in minutes
        float inspection_utility;
        vector<int> opening_hours; // vector<int>(24, 0/1) 0 = closed, 1 = open
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
    vector<Node> nodes; // The list of nodes being represented

    int nrVehicles;    // Number of vehicles

    Time departure_time;    // Time when vehicles start working
    Time max_work_time; // Maximum time vehicles can work
    Time limit_time;    // Time when vehicles stop working
    vector<Time> times;     // Current times for each vehicle

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
    void setNode(int index, string district, string county, string parish, string address, float latitude, float longitude, float inspection_utility, int inspection_time, vector<int> opening_hours);

    /**
     * Get Nodes
     * @return  Nodes
     */
    map<int, pair<float, float>> getNodes();

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
    list<int> path(int origin, int destin);

    /**
     * Evaluate Solution
     * @param solution  Solution
     * @return  Solution Value
     */
    int evaluate_solution(const vector<list<int>>& solution);

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
    int closest_node(int idx, list<int> incompatible) const;

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
    vector<list<int>> generate_random_solution(bool log=false);

    /**
     * Get Solution with closest nodes
     * @return  Random Solution
     */
    vector<list<int>> generate_closest_solution(bool log=false);

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
    static void printSolution(const vector<list<int>>& solution);

    /**
     * Display more details about a solution
     * @param solution
     */
    void printDetailedSolution(const vector<list<int>>& solution);

    /**
     * Get total travel time of a solution
     * @param solution  Solution
     * @return  Total travel time
     */
    float totalTravelTime(const vector<list<int>>& solution);

    /**
     * Get total waiting time of a solution
     * @param solution  Solution
     * @return  Total waiting time
     */
    float totalWaitingTime(const vector<list<int>>& solution);

    /**
     * Get total operation time of a solution
     * @param solution  Solution
     * @return  Total operation time
     */
    float totalOperationTime(const vector<list<int>>& solution);

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
    vector<list<int>> mutation_solution_1(const vector<list<int>>& solution);

    /**
     * Exchange one establishment for one that didn't appear in the solution
     * @param solution
     * @return
     */
    vector<list<int>> mutation_solution_2(const vector<list<int>>& solution);

    /**
      * Generate a new path between two nodes
      * @param solution
      * @return
      */
    vector<list<int>> mutation_solution_3(const vector<list<int>>& solution);

    /**
      * Scramble the order of a subset of nodes in a vehicle's path
      * @param solution
      * @return
      */
    vector<list<int>> mutation_solution_4(const vector<list<int>>& solution);

    /**
      * Remove a node from a vehicle's path and insert two new nodes
      * @param solution
      * @return
      */
    vector<list<int>> mutation_solution_5(const vector<list<int>>& solution);

    /**
     * Choose a random mutation
     * @param solution
     * @return
     */
    vector<list<int>> mutation_solution_6(const vector<list<int>>& solution);

    vector<list<int>> hillClimbing(int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>>&), bool log=false);

    vector<list<int>> simulatedAnnealing(int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>>&), bool log=false);

    void tabuSearch();

    void geneticAlgorithm(int iteration_number, int population_size, vector<list<int>> (Graph::*crossover_func)(const vector<list<int>>&), vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (*evaluation_func)(const vector<list<int>>&), bool log=false);

    bool check_solution(vector<list<int>> vector1);
};

#endif /* GRAPH_H_ */
