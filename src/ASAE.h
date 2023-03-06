#ifndef ASAE_H
#define ASAE_H

#include "graph/graph.h"
#include <map>
#include <iomanip>
#include <iostream>

class ASAE
{
    int number_of_vans = 0; 

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
    void readTimeDistances();

public:
    /**
     * ASAE Constructor
     */
    ASAE();

    /**
     * Set number of vans
     * @param numberOfVans
     */
    void setNumberOfVans(int numberOfVans);

    /**
     * Get number of vans
     * @param numberOfVans
     * @return
     */
    int getNumberOfVans(int numberOfVans);

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


};

#endif // ASAE_H