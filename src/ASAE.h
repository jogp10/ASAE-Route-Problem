#ifndef ASAE_H
#define ASAE_H

#include "graph/graph.h"
#include <map>
#include <iomanip>
#include <iostream>

class ASAE
{
    Graph graph = Graph(0);

    /**
     * Number lines in file
     * @param myfile file path
     * @return number of lines
     */
    static int numberOfLines(const string &myfile);

    /**
     * Goes to file and reads establishments info, adds them as nodes to graph
     */
    void readEstablishments();

    /**
     * Goes to file and connects stops and lines, by adding edges to graph
     * @param code Line Code
     */
    void readTimeDistances();

public:
    /**
     * ASAE Constructor
     */
    ASAE();

    void menu();
};

#endif // ASAE_H