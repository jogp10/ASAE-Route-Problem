//
// Created by john on 20/01/22.
//

#include <cmath>
#include <matplot/matplot.h>
#include "src/ASAE.h"


int main(int argc, char* argv[]) {

    using namespace matplot;

    //std::vector<int> y = {2, 4, 7, 7, 6, 3, 9, 7, 3, 5};
    //plot(y);

    //show();

    ASAE asae = ASAE();
    asae.menu();

    return 0;
}

