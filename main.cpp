//
// Created by john on 20/01/22.
//

#include "src/ASAE.h"
#include "./include/matplotlibcpp.h"

namespace plt = matplotlibcpp;

int main(int argc, char* argv[]) {
    //plt::plot({1,2,3,4},"*");
    //plt::show();

    ASAE asae = ASAE();
    asae.menu();
    
    return 0;
}

