//
// Created by john on 20/01/22.
//

#include <cmath>
#include <matplot/matplot.h>

#include "src/ASAE.h"



int main(int argc, char* argv[]) {
    using namespace matplot;
    std::vector<double> x = linspace(0, 2 * pi);
    std::vector<double> y = transform(x, [](auto x) { return sin(x); });

    plot(x, y, "-o");
    hold(on);
    plot(x, transform(y, [](auto y) { return -y; }), "--xr");
    plot(x, transform(x, [](auto x) { return x / pi - 1.; }), "-:gs");
    plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");

    show();

    ASAE asae = ASAE();
    asae.menu();

    return 0;
}

