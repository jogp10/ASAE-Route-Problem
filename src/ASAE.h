#ifndef ASAE_H
#define ASAE_H

#include <map>
#include <iomanip>
#include <iostream>

#include "graph/graph.h"

using namespace std;

class ASAE
{
    Graph graph = Graph(0);
    string establishmentsFile = "dataset/establishments.csv";
    string distancesFile = "dataset/distances.csv";
    int max_establishments= 0;

    vector<vector<std::list<int>>(Graph::*)(const std::vector<std::list<int>>&)> mutation_funcs = {&Graph::mutation_solution_1, &Graph::mutation_solution_2,&Graph::mutation_solution_3, &Graph::mutation_solution_4,&Graph::mutation_solution_5,&Graph::mutation_solution_6};
    vector<pair<vector<std::list<int>>, vector<list<int>>>(Graph::*)(const std::vector<std::list<int>>&, const std::vector<std::list<int>>&)> crossover_funcs = {&Graph::crossover_solutions_1, &Graph::crossover_solutions_2};
    vector<int(Graph::*)(const std::vector<std::list<int>>&)> evaluation_funcs = {&Graph::evaluate_solution_1, &Graph::evaluate_solution_2};

    /**
     * Number lines in file
     * @param myfile file path
     * @return number of lines
     */
    static int numberOfLines(const string &myfile);

    /**
     * Goes to file and reads establishments info, adds them as nodes to graph
     */
    void readEstablishments(const int &n);

    /**
     * Goes to file and connects stops and lines, by adding edges to graph
     * @param code Line Code
     */
    void readTimeDistances(const int &n);

    /**
     * Checks if string has substring "p_"
     * @param s
     * @return
     */
    static bool hasSubstring(const string &s);

    void setMaxEstablishments(const int &n);

    int getEstablishments(const int &n) const;

public:
    /**
     * ASAE Constructor
     */
    explicit ASAE(int max_establishments = -1);

    /**
     * ASAE menu display
     */
    void menu();

    /**
     * Hill Climbing Algorithm
     */
    void hill_climbing();

    /**
     * Simulated Annealing Algorithm
     */
    void simulated_annealing();

    /**
     * Tabu Search Algorithm
     */
    void tabu_search();

    /**
     * Genetic Algorithm
     */
    void genetic();

    /**
     * Check if a number in the correct interval (a to b) is inputted
     * @param a
     * @param text
     * @return
     */
    static bool parseInput(int a, int b, const string &text);

    /**
     * Checks if a string represents a number
     * @param str
     * @return
     */
    static bool isAllDigits(const string &str);

    static bool ask_parameters(string &iteration_number, string &mutation_func, string &evaluation_func);

    static bool ask_tabu_parameters(string &tabu_size, string &neighborhood_size);

    static void ask_genetic_parameters(string &population_size, string &tournament_size, string &mutation_rate, string &crossover_func);

    static bool print_logs();

    void printEndAlgorithm(const vector<list<int>>& s, int number_of_iterations, int iterations_to_reach_optimal, float total_time, float time_to_reach_optimal);

    void compare_algorithms();

    void plots();

    int askVehicleToPlot();
};

#endif // ASAE_H