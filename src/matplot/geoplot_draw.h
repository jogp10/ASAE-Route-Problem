//
// Created by ricar on 3/16/2023.
//

#ifndef ASAEINSPECTIONROUTE_GEOPLOT_DRAW_H
#define ASAEINSPECTIONROUTE_GEOPLOT_DRAW_H

#include <vector>
#include <random>



#include <string>
#include <matplot/matplot.h>
#include "../graph/graph.h"

using namespace matplot;
using namespace std;

class Geoplot_draw {
public:
    Geoplot_draw(Graph graph);
    void run(size_t iterations = 100);

    void simple();

private:
    void setup_axes();
    void setup_starting_point(size_t iteration);
    double tour_distance(const vector<size_t> &tour);
    void iteration();
    static vector<vector<size_t>>
    get_neighbors(const std::vector<size_t> &tour);
    bool update_if_better(const vector<size_t> &neighbor);
    void draw_if_improvement();
    void draw();

private:
    Graph graph;

    // Current tour
    double curr_dist_{0.0};
    vector<size_t> curr_tour_;

    // Best tour
    double min_dist_{0.0};
    vector<size_t> best_tour_;

};


#endif //ASAEINSPECTIONROUTE_GEOPLOT_DRAW_H
