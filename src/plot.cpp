//
// Created by john on 15-03-2023.
//

#include "plot.h"

using namespace matplot;

plot::plot() {
    std::cout << "plot constructor" << std::endl;
}

void plot::plotGraph() {
    std::cout << "plot graph" << std::endl;

    std::vector<double> x = {1, 2, 3, 4, 5};
    std::vector<double> y = {1, 4, 9, 16, 25};
    matplot::plot(x, y);

    matplot::show();
    return;
}
