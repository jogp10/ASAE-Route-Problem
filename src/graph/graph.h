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

using namespace std;


class Graph {

    struct Edge {
        int dest;   // Destination node
        float weight; // An integer weight
    };

    struct Node {
        list<Edge> adj; // The list of outgoing edges (to adjacent nodes)
        int pred = 0;
        bool visited;
        float dist;

        int id;
        string district;
        string county;
        string parish;
        string address;
        float latitude;
        float longitude;
        int inspection_time;
        float inspection_utility;
        vector<int> opening_hours;
    };

    struct Time {
        int milliseconds;
        int seconds;
        int minutes;
        int hours;

        void setTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0);

        void addTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0);
        void addTime(Time time);
        void subTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0);
        void subTime(Time time);

        void toNextHour();

        bool operator<(const Time &rhs) const;

        void toPreviousHour();
    };

    int n;              // Graph size (vertices are numbered from 1 to n)
    bool hasDir;        // false: undirected; true: directed
    vector<Node> nodes; // The list of nodes being represented

    int nrVehicles;

    Time departure_time;
    Time max_work_time;
    Time limit_time;
    vector<Time> times;

    void setNrVehicles(int n) { nrVehicles = floor(n*0.1);};


public:
    /**
     *  Constructor
     * @param nodes nr nodes
     * @param dir direction ( default: undirected)
     */
    explicit Graph(int nodes, bool dir = false, Time departure_time = {0, 0, 0, 0}, Time max_work_time = {0, 0, 0, 0});

    /**
     * Add edge from source to destination with a certain weight
     * @param src Origin Stop
     * @param dest Destination Stop
     * @param weight Distance between Origin and Destination
     */
    void addEdge(int src, int dest, float weight = 1.00);

    /**
     * Set Node's updated with Stop's info
     * @param Name Name of Stop
     * @param Code Code of Stop
     * @param Zone Zone of Stop
     * @param Latitude Latitude of Stop
     * @param Longitude Longitude of Stop
     * @param index Index in vector<Node> of Graph
     */
    void setNode(int index, string address, float latitude, float longitude, float inspection_utility, int inspection_time, vector<int> opening_hours);

    /**
     * Get Map of nodes which belong to graph
     * @return map where key: code of stop, value: latitude, longitude
     */
    map<int, pair<float, float>> getNodes();

    /**
     * Get Node
     * @param at index of Node
     * @return Node
     */
    Node getNode(int at);

    /**
     * Path from a to b
     * @param a
     * @param b
     * @return
     */
    list<int> path(int a, int b);

    static int evaluate_solution(const vector<list<int>>& solution);

    int random_node(int idx) const;

    int closest_node(int idx) const;

    vector<list<int>> generate_random_solution();

    void hillClimbing();

    void simulatedAnnealing();

    void tabuSearch();

    void geneticAlgorithm();

    float getDistance(int a, int b);

    void showAllEstablishments();

    static void printSolution(const vector<list<int>>& solution);
};

#endif /* GRAPH_H_ */
