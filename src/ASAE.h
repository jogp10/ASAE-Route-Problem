#ifndef ASAE_H
#define ASAE_H

#include "graph/graph.h"
#include <map>
#include <iomanip>
#include <iostream>

class ASAE
{
    map<string, string> lines; // key: codigo da linha, value: nome da linha
    map<string, int> stops;    // key: codigo da paragem, value: index no vetor de nodes do grafo
    Graph graph = Graph(0);
    int hours;
    int minutes;
    int seconds;

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
    void readEdges(const string &code);

public:
    /**
     * ASAE Constructor
     */
    ASAE();

    /**
     * 
     */
    void toRead();

    /**
     * Set time
     * @param time
     */
    void setTime(int seconds = 0, int minutes = 0, int hours = 0);

    /**
     * Add time
     * @param time
     */
    void addTime(int seconds = 0, int minutes = 0, int hours = 0);

    /**
     *
     * Convert code of stop to index in vector<Node> nodes that belongs to graph
     * @param code
     * @return index
     */
    int convertCodeToIndex(string code);
};

#endif // ASAE_H