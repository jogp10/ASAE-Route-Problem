//
// Created by ricar on 3/16/2023.
//

#include "geoplot_draw.h"


Geoplot_draw::Geoplot_draw(Graph graph)
        : graph(graph) {

}






double Geoplot_draw::tour_distance(const vector<size_t> &tour) {

}

void Geoplot_draw::draw() {

}



void Geoplot_draw::setup_axes() {

}

void Geoplot_draw::simple() {
    auto x = std::vector<float>();
    auto y = std::vector<float>();
    double max_lat = -1000;
    double min_lat = 1000;
    double max_lon = -1000;
    double min_lon = 1000;
    int i = 0;
    for (auto &n: graph.getNodes()){
        auto lat = n.second.first;
        auto lon = n.second.second;
        x.push_back(lat);
        y.push_back(lon);
        if(lat > max_lat) max_lat = lat;
        if(lat < min_lat) min_lat = lat;
        if(lon > max_lon) max_lon = lon;
        if(lon < min_lon) min_lon = lon;
        i++;
        if (i==100) break;
    }


    auto g = geoplot(x,
                     y);
    g->line_width(2.);
    hold(on);

    geolimits({min_lat, max_lat}, {min_lon, max_lon});


    color_array terrain = {0.f, 0.71f, 0.65f, 0.59f};
    geoplot()->color(terrain);

    color_array blue_water = {0.f, 0.4f, 0.61f, 0.76f};
    gca()->color(blue_water);

    show();
    return;
}