#ifndef ASAE_H
#define ASAE_H

#include "graph/graph.h"
#include <map>
#include <iomanip>
#include <iostream>

class ASAE
{
    Graph graph = Graph(0);
    string establishmentsFile = "../../dataset/establishments.csv";
    string distancesFile = "../../dataset/distances.csv";
    int max_establishments= 0;

    /**
     * Number lines in file
     * @param myfile file path
     * @return number of lines
     */
    static int numberOfLines(const string &myfile);

    /**
     * Goes to file and reads establishments info, adds them as nodes to graph
     */
    void readEstablishments(int n);

    /**
     * Goes to file and connects stops and lines, by adding edges to graph
     * @param code Line Code
     */
    void readTimeDistances(int n);

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
};

#endif // ASAE_H