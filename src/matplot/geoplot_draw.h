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
    Geoplot_draw(Graph graph, axes_handle ax);
    void run(size_t iterations = 100);

    void draw_all_vehicles(vector<list<int>> solution);
    void draw_one_vehicle(list<int> solution);

private:
    Graph graph;
    axes_handle ax_;
    line_handle lh_;
    void setup_axes();
    void setup_starting_point(size_t iteration);
    double tour_distance(const vector<size_t> &tour);
    void iteration();
    static vector<vector<size_t>>
    get_neighbors(const std::vector<size_t> &tour);
    bool update_if_better(const vector<size_t> &neighbor);
    void draw_if_improvement();
    void draw();




};


#endif //ASAEINSPECTIONROUTE_GEOPLOT_DRAW_H
