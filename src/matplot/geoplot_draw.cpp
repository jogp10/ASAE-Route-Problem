//
// Created by ricar on 3/16/2023.
//

#include <set>
#include <utility>
#include "geoplot_draw.h"


Geoplot_draw::Geoplot_draw(Graph graph,axes_handle ax)
        : graph(std::move(graph)), ax_(std::move(ax)) {

}






double Geoplot_draw::tour_distance(const vector<size_t> &tour) {
    return -1;
}

void Geoplot_draw::draw() {
    }



void Geoplot_draw::setup_axes() {

}

void Geoplot_draw::draw_all_vehicles(const vector<list<int>>& solution) {
    auto lats = std::vector<double>();
    auto longs = std::vector<double>();
    auto addresses = std::vector<std::string>();
    double max_lat = -1000;
    double min_lat = 1000;
    double max_lon = -1000;
    double min_lon = 1000;


    for (const auto& vehicle_path : solution){
        for (auto &n: vehicle_path){
            auto lat = graph.getNode(n).getLatitude();
            auto lon = graph.getNode(n).getLongitude();
            auto address = graph.getNode(n).getAddress();

            lats.push_back(lat);
            longs.push_back(lon);
            addresses.push_back(address);

            if(lat > max_lat) max_lat = lat;
            if(lat < min_lat) min_lat = lat;
            if(lon > max_lon) max_lon = lon;
            if(lon < min_lon) min_lon = lon;


        }

    }



    geoplot(lats, longs)->marker("o").marker_size(5).marker_colors(iota(0, 1.0, lats.size())).display_name("Vehicle");

    hold(on);

    double zoom = 0.06;
    geolimits({min_lat-zoom, max_lat+zoom}, {min_lon-zoom, max_lon+zoom});


    color_array terrain = {0.f, 0.71f, 0.65f, 0.59f};
    geoplot()->color(terrain);

    color_array blue_water = {0.f, 0.4f, 0.61f, 0.76f};
    gca()->color(blue_water);

    int time = int(round(graph.totalOperationTime(solution,false)));

    this->ax_->title("Total Operation Time: "+ num2str(time) + " seconds");
    this->ax_->xlabel("Longitude");
    this->ax_->ylabel("Latitude");
    this->ax_->font("Calibri");
    sgtitle("Random Solution");
    ax_->draw();



    show();

}

void Geoplot_draw::draw_one_vehicle(const list<int>& vehicle_path) {
    auto lats = std::vector<double>();
    auto longs = std::vector<double>();
    auto addresses = std::vector<std::string>();
    double max_lat = -1000;
    double min_lat = 1000;
    double max_lon = -1000;
    double min_lon = 1000;


    for (auto &n: vehicle_path){
        auto lat = graph.getNode(n).getLatitude();
        auto lon = graph.getNode(n).getLongitude();
        auto address = graph.getNode(n).getAddress();

        lats.push_back(lat);
        longs.push_back(lon);
        addresses.push_back(address);

        if(lat > max_lat) max_lat = lat;
        if(lat < min_lat) min_lat = lat;
        if(lon > max_lon) max_lon = lon;
        if(lon < min_lon) min_lon = lon;


    }


    geoplot(lats, longs)->marker("o").marker_size(5).color("yellow");
    text(longs, lats, addresses)->absolute_size(true).font_size(10);
    geolimits({min_lat-0.05, max_lat+0.05}, {min_lon-0.05, max_lon+0.05});


    color_array terrain = {0.f, 0.71f, 0.65f, 0.59f};
    geoplot()->color(terrain);

    color_array blue_water = {0.f, 0.4f, 0.61f, 0.76f};
    gca()->color(blue_water);
    show();

}