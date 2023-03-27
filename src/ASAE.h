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

public:
    /**
     * ASAE Constructor
     */
    ASAE();

    /**
     * ASAE menu display
     */
    void menu();

    /**
     * Checks if string has substring "p_"
     * @param s
     * @return
     */
    static bool hasSubstring(const string &s);

    void drawPlot();

    void hill_climbing();

    void simulated_annealing();

    void tabu_search();

    void genetic();

    bool parseInput(int a, int b, const string &text);

    bool isAllDigits(const string &str);

    bool ask_parameters(string &iteration_number, string &mutation_func, string &evaluation_func);

    bool print_logs();

    bool ask_tabu_parameters(string &tabu_size, string &neighborhood_size);

    void ask_genetic_parameters(string &population_size, string &tournament_size, string &mutation_rate);
};

#endif // ASAE_H