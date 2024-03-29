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
    explicit Geoplot_draw(Graph graph);

    void draw_all_vehicles(const vector<list<int>>& solution, const string& title="Solution");
    void draw_one_vehicle(const list<int>& solution);

    void evolution_graph(vector<int> vector1, const string& title);

    void compare_algorithms(std::vector<int> sol1, std::vector<int> sol2,
                            std::vector<int> sol3, std::vector<int> sol4, int num_iterations);

private:
    Graph graph;
    axes_handle ax_;
    line_handle lh_;
    void setup_axes();



    void draw();


};


#endif //ASAEINSPECTIONROUTE_GEOPLOT_DRAW_H
