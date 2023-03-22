#include "graph.h"
#include "minHeap.h"
#include "../matplot/geoplot_draw.h"

#include <random>
#include <utility>
#include <list>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;
// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir, Time departure_time, Time max_work_time) : n(num), hasDir(dir), nodes(num), engine(std::random_device{}()) {
    this->departure_time = departure_time;
    this->max_work_time = max_work_time;
    this->limit_time = this->departure_time;
    this->limit_time.addTime(this->max_work_time);
    this->setNrVehicles(n);
    this->times = vector<Time>(nrVehicles, this->departure_time);

}


// Add edge from source to destination with a certain weight
void Graph::addEdge(int src, int dest, float weight) {
    if (src<0 || src>n || dest<0 || dest>n || src==dest) return;
    nodes[src].adj.push_back({dest, weight});
    if (!hasDir) nodes[dest].adj.push_back({src, weight});
}


map<int, pair<float, float>> Graph::getNodes(){
    map<int, pair<float, float>> localizations;
    for(int i=1; i<nodes.size(); ++i){
        localizations.insert(make_pair(i, make_pair(nodes[i].latitude, nodes[i].longitude)));
    }
    return localizations;
}


Graph::Node Graph::getNode(int at){
    return nodes[at];
}


list<int> Graph::path(int a, int b) {
    list<int> path;
    int u=b;

    while(a!=u){
        path.insert(path.begin(), u);
        if(u==nodes[u].pred || !(nodes[u].pred>0 && nodes[u].pred<=nodes.size())) break;
        u=nodes[u].pred;
    }
    path.insert(path.begin(), a);
    return path;
}


void Graph::setNode(int index, string district, string county, string parish, string address, float latitude, float longitude, float inspection_utility, int inspection_time, vector<int> opening_hours) {
    Node u;
    u.id = index;
    u.district = std::move(district);
    u.county = std::move(county);
    u.parish = std::move(parish);
    u.address = std::move(address);
    u.latitude = latitude;
    u.longitude = longitude;
    u.inspection_time = inspection_time;
    u.inspection_utility = inspection_utility;
    u.opening_hours = std::move(opening_hours);
    u.dist = INT_MAX;
    nodes[index] = u;
}


int Graph::evaluate_solution(const vector<list<int>>& solution) {
    int bestSolution = 0;
    for (const auto & i : solution){
        bestSolution += i.size();
    }
    return bestSolution;
}


int Graph::random_node(const int idx) const {
    while (true) {
        int random = rand() % (n-1) + 1;
        if (random != idx) {
            return random;
        }
    }
}


vector<list<int>> Graph::generate_random_solution(bool log) {

    int nr_visited = 0;
    vector<list<int>> not_compatible_pairs(nrVehicles, list<int>());
    vector<list<int>> solution(nrVehicles, list<int>(1, 0));
    int max_tries = n*10;

    for (auto &t: times) t = departure_time;
    for (auto &n: nodes) n.visited = false;

    while (nr_visited < n && max_tries) {
        int i = engine() % nrVehicles;
        int j = (engine() % (n - 1)) + 1;

        if (nodes[j].visited) {
            max_tries--;
            continue;
        }

        if (find(not_compatible_pairs[i].begin(), not_compatible_pairs[i].end(), j) != not_compatible_pairs[i].end()) {
            max_tries--;
            continue;
        }

        int last = solution[i].back();

        if(log) cout << "Vehicle -> " << i << "    Node -> " << last << endl;
        if(log) cout << times[i].hours << ":" << times[i].minutes << "h" << endl;

        Time min_op = minimumOperationTime(last, j, times[i]);
        min_op.addTime(times[i]);
        if (limit_time < min_op) {
            if(log) cout << "Limit time reached" << endl;
            not_compatible_pairs[i].emplace_back(j);
            continue;
        }

        nr_visited++;
        nodes[j].visited = true;
        solution[i].push_back(j);
        Time op = operationTime(last, j, times[i]);
        times[i].addTime(op);
    }

    for (int i = 0; i < nrVehicles; ++i) { solution[i].push_back(0); } // add depot to the end of each route

    return solution;
}


vector<list<int>> Graph::generate_closest_solution(bool log) {
    vector<list<int>> solution(nrVehicles, list<int>(1, 0));
    vector<MinHeap<int, int>> heaps(n, MinHeap<int, int>(n, -1));

    for (auto &t: times) t = departure_time;
    for (auto &n: nodes) {
        for(auto &e: n.adj) {
            heaps[n.id].insert(e.dest, e.weight);
        }
        n.visited = false;
    }
    nodes[0].visited = true;

    for(int i=0; i<nrVehicles; i++) {
        int limit = 5;

        if(i >= solution.size()) break;

        for(int j=0; j<n-2; j++) {
            int last = solution[i].back();
            int nthClosest = heaps[last].removeMin();

            if (nodes[nthClosest].visited) continue;
            if(limit == 0) break;

            if(log) cout << "Vehicle -> " << i << "    Node -> " << last << endl;
            if(log) cout << times[i].hours << ":" << times[i].minutes << "h" << endl;

            Time min_op = minimumOperationTime(last, nthClosest, times[i], log);

            min_op.addTime(times[i]);
            if (limit_time < min_op) {
                limit--;
                if(log) cout << "Limit time reached" << endl;
                continue;
            }

            nodes[nthClosest].visited = true;
            solution[i].push_back(nthClosest);
            Time op = operationTime(last, nthClosest, times[i], log);
            times[i].addTime(op);
            j=-1;
        }
    }

    for (int i = 0; i < nrVehicles; ++i) { solution[i].push_back(0); } // add depot to the end of each route

    return solution;
}


void Graph::showAllEstablishments() {
    for (int i = 1; i < nodes.size(); ++i) {
        cout << "Node " << i << ": " << nodes[i].address << endl;
    }
}


void Graph::printSolution(const vector<list<int>> &solution) {
    for (int i = 0; i < solution.size(); ++i) {
        cout << "Vehicle " << i << ": ";
        for (auto it = solution[i].begin(); it != solution[i].end(); ++it) {
            cout << *it;
            if(next(it) != solution[i].end()) cout << " -> ";
        }
        cout << endl;
    }
}


int Graph::closest_node(int idx, list<int> incompatible) const {
    int closest = -1;
    float min_dist = INT_MAX;
    for(auto e: nodes[idx].adj){
        if(e.weight < min_dist && find(incompatible.begin(), incompatible.end(), e.dest) == incompatible.end() && e.dest > 0){
            min_dist = e.weight;
            closest = e.dest;
        }
    }
    return closest;
}


int Graph::closest_node(int idx, int order) const {
    order = order > n-2 ? n-2 : order;

    vector<int> closest_nodes(order, -1);
    vector<float> closest_nodes_dist(order, INT_MAX);

    for(int i = 0; i < order; ++i) {
        for (auto e: nodes[idx].adj) {
            if (e.weight < closest_nodes_dist[i] && find(closest_nodes.begin(), closest_nodes.end(), e.dest) == closest_nodes.end() && e.dest != idx && e.dest > 0) {
                closest_nodes_dist[i] = e.weight;
                closest_nodes[i] = e.dest;
            }
        }
    }

    return closest_nodes[order-1];
}


float Graph::getDistance(int a, int b) {
    for (auto e: nodes[a].adj) {
        if (e.dest == b) {
            return e.weight;
        }
    }
    return -1.0;
}


float Graph::totalTravelTime(const vector<list<int>> &solution, bool log) {
    float travel_time = 0;
    for (int i = 0; i < solution.size(); ++i) {
        float vehicle_time = 0;
        int last = 0;
        for (auto it = solution[i].begin(); it != solution[i].end(); ++it)  {
            vehicle_time += getDistance(last, *it);
            last = *it;
        }
        cout << "Vehicle " << i << " total travel time: " << vehicle_time << "s" << endl;
        travel_time += vehicle_time;
    }
    cout << "Total travel time: " << travel_time << "s" << endl;
    return travel_time;
}


float Graph::totalWaitingTime(const vector<list<int>> &solution, bool log) {
    float waiting_time = 0;

    for (int i = 0; i < solution.size(); ++i) {
        float vehicle_time = 0;
        Time time = departure_time;
        int last = 0;

        auto it = solution[i].begin();
        it++;
        for (; it != solution[i].end(); ++it)  {
            int seconds = (int) getDistance(last, *it);
            int miliseconds = (int) ((getDistance(last, *it) - (float) seconds) * 1000);
            time.addTime({miliseconds, seconds, 0, 0});
            while (nodes[*it].opening_hours[time.hours] == 0) {
                auto aux = time;
                time.toNextHour();
                auto diff = time;
                diff.subTime(aux);
                vehicle_time += diff.toSeconds();
            }
            time.addTime(0, 0, nodes[*it].inspection_time, 0);
            last = *it;
        }
        cout << "Vehicle " << i << " total waiting time: " << vehicle_time << "s" << endl;
        waiting_time += vehicle_time;
    }
    cout << "Total waiting time: " << waiting_time << "s" << endl;
    return waiting_time;
}


Time Graph::minimumOperationTime(int a, int b, Time time, bool log) {
    Time aux = time;

    float time_distance = (int) getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    float time_depot = (int) getDistance(b, 0);
    int milliseconds_distance = (int) ((getDistance(a, b) - (float) time_distance) * 1000);
    int milliseconds_depot = (int) ((getDistance(b, 0) - (float) time_depot) * 1000);

    Time w = {0, 0, 0, 0};
    w.addTime({milliseconds_distance, (int) time_distance, 0, 0});
    if(log)  cout << "Time to travel to Node " << b << ": " << w.hours << ":" << w.minutes << "h" << endl;

    time.addTime({milliseconds_distance, (int) time_distance, 0, 0});

    if(nodes[b].opening_hours[time.hours] == 0 && log) cout << "Node " << b << " is closed at " << time.hours << endl;
    for(int i = time.hours; i < limit_time.hours+1; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }
    if(log) cout << "Node " << b << " is open /(time) " << time.hours << ":" << time.minutes << "h" << endl;

    time.addTime({milliseconds_depot, (int) time_depot, time_inspection, 0});
    if(log) cout << "Inspection time: " << time_inspection/60 << ":" << time_inspection%60 << endl;
    time.subTime(aux);
    return time;
}


Time Graph::operationTime(int a, int b, Time time, bool log) {
    Time aux = time;

    float time_distance = (int) getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    int milliseconds_distance = (int) ((getDistance(a, b) - (float) time_distance) * 1000);

    time.addTime({milliseconds_distance, (int) time_distance, 0, 0, 0});

    for(int i = time.hours; i < limit_time.hours+1; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }

    if(b!=0)time.addTime({0, 0, time_inspection, 0, 0});

    time.subTime(aux);

    return time;
}


float Graph::totalOperationTime(const vector<list<int>> &solution, bool log) {
    float operation_time = 0;
    int number_above = 0;
    for (int i = 0; i < solution.size(); ++i) {
        Time t = departure_time;
        int last = 0;

        auto it = solution[i].begin();
        it++;
        for (; it != solution[i].end(); ++it)  {
            t.addTime(operationTime(last, *it, t));
            last = *it;
        }

        t.subTime(departure_time);
        float vehicle_time = t.toSeconds();
        cout << "Vehicle " << i << " total operation time: " << vehicle_time << "s" << endl;
        operation_time += vehicle_time;
        if(vehicle_time > 28800) number_above++;
    }
    cout << "Total operation time: " << operation_time << "s" << endl;
    cout << "Number of vehicles above 8h: " << number_above << endl;
    return operation_time;
}


void Graph::printDetailedSolution(const vector<list<int>> &solution, bool log) {
    // For each step in vechicle i, node,time before leaving to node, time of travel distance to node, inspection time, time after inspection
    for (int i = 0; i < solution.size(); ++i) {
        Time t = departure_time;
        int last = 0;

        auto it = solution[i].begin();
        it++;
        for (; it != solution[i].end(); ++it)  {
            cout << "Vehicle " << i << " Going to Node " << *it << " " << t.hours << ":" << t.minutes << "h" << endl;
            cout << "Time to travel to Node " << *it << " " << (int) getDistance(last, *it)/60 << "m" << endl;
            cout << "Time to inspect Node " << *it << ": " << nodes[*it].inspection_time/60 << ":" << nodes[*it].inspection_time%60 << endl << endl;
            t.addTime(operationTime(last, *it, t, log));
            last = *it;
        }
    }

}


void Graph::setNrVehicles(int n) {
        nrVehicles = floor(n*0.1);
        if(nrVehicles == 0) nrVehicles = 1;
}


vector<list<int>> Graph::mutation_solution_1(const vector<list<int>> &solution) {
    vector<list<int>> new_solution = solution;
    int vehicle = engine() % nrVehicles;
    int vehicle2 = engine() % nrVehicles;
    int node = engine() % (new_solution[vehicle].size() - 2) + 1;
    int node2 = engine() % (new_solution[vehicle2].size() - 2) + 1;
    while(vehicle == vehicle2 && node ==node2){
        node2 = engine() % (new_solution[vehicle2].size() - 2) + 1;
    }


    auto it = new_solution[vehicle].begin();
    auto it2 = new_solution[vehicle2].begin();

    advance(it, node);
    advance(it2, node2);

    int aux = *it;
    *it = *it2;
    *it2 = aux;
    return check_solution(new_solution) ? new_solution : solution;
}

vector<list<int>> Graph::mutation_solution_2(const vector<list<int>> &solution) {
    vector<list<int>> new_solution = solution;
    int vehicle = engine() % nrVehicles;
    int node = engine() % (new_solution[vehicle].size() - 2) + 1;
    int node2 = engine() % (nodes.size() - 1) + 1;

    for (int i = 0; i < new_solution.size(); i++) {
        for (auto it = new_solution[i].begin(); it != new_solution[i].end(); ++it) {
            if (*it == node2) {
                node2 = engine() % (nodes.size() - 1) + 1;
                i = -1;
                break;
            }
        }
    }

    auto it = new_solution[vehicle].begin();
    advance(it, node);

    *it = node2;

    return check_solution(new_solution) ? new_solution : solution;
}

vector<list<int>> Graph::mutation_solution_3(const vector<list<int>>& solution) {
    vector<list<int>> new_solution = solution;
    return check_solution(new_solution) ? new_solution : solution;
}


vector<list<int>> Graph::mutation_solution_4(const vector<list<int>> &solution) {
    vector<list<int>> new_solution = solution;

    int vehicle = engine() % nrVehicles;
    int node = engine() % (new_solution[vehicle].size() - 2) + 1;
    int node2 = engine() % (new_solution[vehicle].size() - 2) + 1;

    if(solution[vehicle].size() < 5) return solution;

    while(abs(node - node2) < 2) {
        node = engine() % (new_solution[vehicle].size() - 2) + 1;
        node2 = engine() % (new_solution[vehicle].size() - 2) + 1;
    }

    if(node > node2) {
        int aux = node;
        node = node2;
        node2 = aux;
    }

    auto it = next(new_solution[vehicle].begin(), node);
    auto it2 = next(new_solution[vehicle].begin(), node2);

    vector<int> aux(it, it2);
    shuffle(aux.begin(), aux.end(), default_random_engine(engine()));
    copy(aux.begin(), aux.end(), it);

    return check_solution(new_solution) ? new_solution : solution;
}

vector<list<int>> Graph::mutation_solution_5(const vector<list<int>> &solution) {
    vector<list<int>> new_solution = solution;

    int vehicle = engine() % nrVehicles;
    int node = engine() % (new_solution[vehicle].size() - 2) + 1;
    int node2 = engine() % (nodes.size() - 1) + 1;
    int node3 = engine() % (nodes.size() - 1) + 1;

    for (int i = 0; i < new_solution.size(); i++) {
        for (auto it = new_solution[i].begin(); it != new_solution[i].end(); ++it) {
            if (*it == node2) {
                node2 = engine() % (nodes.size() - 1) + 1;
                i = -1;
                break;
            }
        }
    }

    for (int i = 0; i < new_solution.size(); i++) {
        for (auto it = new_solution[i].begin(); it != new_solution[i].end(); ++it) {
            if (*it == node3 || node2 == node3) {
                node3 = engine() % (nodes.size() - 1) + 1;
                i = -1;
                break;
            }
        }
    }

    auto it = new_solution[vehicle].begin();
    advance(it, node);

    *it = node2;
    new_solution[vehicle].insert(it, node3);

    return check_solution(new_solution) ? new_solution : solution;
}

vector<list<int>> Graph::mutation_solution_6(const vector<list<int>> &solution) {
    int mutation = engine() % 5 + 1;
    switch (mutation) {
        case 1:
            return mutation_solution_1(solution);
        case 2:
            return mutation_solution_2(solution);
        case 3:
            return mutation_solution_3(solution);
        case 4:
            return mutation_solution_4(solution);
        case 5:
            return mutation_solution_5(solution);
        default:
            return solution;
    }
}

/**
 * Determine shortest path in both solutions to help select midpoint in crossover functions
 * @param father_solution
 * @param mother_solution
 * @return shortest path length
 */
int shortest_path_size(vector<list<int>> father_solution, vector<list<int>> mother_solution) {
    int shortest = INT32_MAX;

    for(int i = 0; i < father_solution.size(); i++) { // both solutions have the same size
        if(father_solution[i].size() < shortest) shortest = father_solution[i].size();
        if(mother_solution[i].size() < shortest) shortest = mother_solution[i].size();
    }

    return shortest;
}

void Graph::fillSolution(vector<list<int>> &child) {
    vector<MinHeap<int, int>> heaps(n, MinHeap<int, int>(n, -1));

    for (auto &n: nodes) {
        for(auto &e: n.adj) {
            heaps[n.id].insert(e.dest, e.weight);
        }
        n.visited = false;
    }
    nodes[0].visited = true;

    for(int i=0; i<nrVehicles; i++) {
        int limit = 5;

        if(i >= child.size()) break;

        for(int j=0; j<n-2; j++) {
            int last = child[i].back();
            int nthClosest = heaps[last].removeMin();

            if (nodes[nthClosest].visited) continue;
            if(limit == 0) break;

            Time min_op = minimumOperationTime(last, nthClosest, times[i], false);

            min_op.addTime(times[i]);
            if (limit_time < min_op) {
                limit--;
                continue;
            }

            nodes[nthClosest].visited = true;
            child[i].push_back(nthClosest);
            Time op = operationTime(last, nthClosest, times[i], false);
            times[i].addTime(op);
            j=-1;
        }
    }
}

/*
 * Crossover 1: Select a midpoint, smaller than the shortest path in the solution, which will be dividing the both
 * solutions in the midpoint. The two new resultant solutions consist in changing the cuts of the parents solutions.
 */
pair<vector<list<int>>, vector<list<int>>> Graph::crossover_solutions_1(vector<list<int>> father_solution, vector<list<int>> mother_solution) {
    int midpoint = shortest_path_size(father_solution, mother_solution);
    vector<list<int>> child1, child2;

    for(int i = 0; i < father_solution.size(); i++) {

        if(i < midpoint) {
            child1.push_back(father_solution[i]);
            child2.push_back(mother_solution[i]);
        }
        else {
            child1.push_back(mother_solution[i]);
            child2.push_back(father_solution[i]);
        }

    }

    set<int> used_establishments1;

    for(auto van: child1) {
        for(list<int>::iterator it = van.begin(); it != van.end(); it++) {
            int previous_size = used_establishments1.size();
            used_establishments1.insert(*it);

            if(previous_size == used_establishments1.size()) {
                van.erase(it);
                it = van.begin();
            }
        }
    }

    set<int> used_establishments2;

    for(auto van: child2) {
        for(list<int>::iterator it = van.begin(); it != van.end(); it++) {
            int previous_size = used_establishments2.size();
            used_establishments2.insert(*it);

            if(previous_size == used_establishments2.size()) {
                van.erase(it);
                it = van.begin();
            }
        }
    }

    /*
    cout << "==== BEFORE ====" << endl;
    cout << "score father: " << this->evaluate_solution(father_solution) << endl;
    cout << "score mother: " << this->evaluate_solution(mother_solution) << endl;
    cout << "valid 1: " << this->check_solution(child1) << endl;
    cout << "score 1: " << this->evaluate_solution(child1) << endl;
    cout << "valid 2: " << this->check_solution(child2) << endl;
    cout << "score 2: " << this->evaluate_solution(child2) << endl;

    fillSolution(child1);
    fillSolution(child2);

    cout << "==== AFTER ====" << endl;
    cout << "score father: " << this->evaluate_solution(father_solution) << endl;
    cout << "score mother: " << this->evaluate_solution(mother_solution) << endl;
    cout << "valid 1: " << this->check_solution(child1) << endl;
    cout << "score 1: " << this->evaluate_solution(child1) << endl;
    cout << "valid 2: " << this->check_solution(child2) << endl;
    cout << "score 2: " << this->evaluate_solution(child2) << endl;
    */

    fillSolution(child1);
    fillSolution(child2);

    return make_pair(child1, child2);
}

/*
 * Crossover 2: Take the middle part of the first parent’s solution between two crossover points and filling the
 * remaining parts with the nodes from the second parent’s solution, creating the child solutions.
 */
pair<vector<list<int>>, vector<list<int>>> Graph::crossover_solutions_2(vector<list<int>> father_solution, vector<list<int>> mother_solution) {
    int midpoint1 = (rand() % (father_solution.size() - 1)) + 0;
    int midpoint2 = (rand() % (father_solution.size() - 1)) + midpoint1;

    vector<list<int>> child1, child2;

    for(int i = 0; i < father_solution.size(); i++) {
        if(i < midpoint1) {
            child1.push_back(father_solution[i]);
            child2.push_back(mother_solution[i]);
        }
        else if(i > midpoint1 && i < midpoint2) {
            child1.push_back(mother_solution[i]);
            child2.push_back(father_solution[i]);
        }
        else if(i > midpoint2) {
            child1.push_back(father_solution[i]);
            child2.push_back(mother_solution[i]);
        }
    }

    set<int> used_establishments1;

    for(auto van: child1) {
        for(list<int>::iterator it = van.begin(); it != van.end(); it++) {
            int previous_size = used_establishments1.size();
            used_establishments1.insert(*it);

            if(previous_size == used_establishments1.size()) {
                van.erase(it);
                it = van.begin();
            }
        }
    }

    set<int> used_establishments2;

    for(auto van: child2) {
        for(list<int>::iterator it = van.begin(); it != van.end(); it++) {
            int previous_size = used_establishments2.size();
            used_establishments2.insert(*it);

            if(previous_size == used_establishments2.size()) {
                van.erase(it);
                it = van.begin();
            }
        }
    }

    /*
    cout << "==== BEFORE ====" << endl;
    cout << "score father: " << this->evaluate_solution(father_solution) << endl;
    cout << "score mother: " << this->evaluate_solution(mother_solution) << endl;
    cout << "valid 1: " << this->check_solution(child1) << endl;
    cout << "score 1: " << this->evaluate_solution(child1) << endl;
    cout << "valid 2: " << this->check_solution(child2) << endl;
    cout << "score 2: " << this->evaluate_solution(child2) << endl;

    fillSolution(child1);
    fillSolution(child2);

    cout << "==== AFTER ====" << endl;
    cout << "score father: " << this->evaluate_solution(father_solution) << endl;
    cout << "score mother: " << this->evaluate_solution(mother_solution) << endl;
    cout << "valid 1: " << this->check_solution(child1) << endl;
    cout << "score 1: " << this->evaluate_solution(child1) << endl;
    cout << "valid 2: " << this->check_solution(child2) << endl;
    cout << "score 2: " << this->evaluate_solution(child2) << endl;
    */

    fillSolution(child1);
    fillSolution(child2);

    return make_pair(child1, child2);
}

vector<list<int>> Graph::hillClimbing(const int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {
    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    int iteration = 0;

    while(iteration < iteration_number) {
        iteration++;

        vector<list<int>> neighbour_solution = (this->*mutation_func)(best_solution);
        int neighbour_score = (this->*evaluation_func)(neighbour_solution);

        if(neighbour_score > best_score) {
            best_solution = neighbour_solution;
            best_score = neighbour_score;
        }

    }

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;
    return best_solution;
}


vector<list<int>> Graph::simulatedAnnealing(const int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {
    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    int iteration = 0;
    float temperature = 1000;

    while(iteration < iteration_number) {
        temperature *= 0.999;
        iteration++;

        vector<list<int>> neighbour_solution = (this->*mutation_func)(best_solution);
        int neighbour_score = (this->*evaluation_func)(neighbour_solution);
        float r = static_cast <float> (engine()) / static_cast <float> (RAND_MAX);


        if((neighbour_score > best_score) or (pow(M_E, (neighbour_score - best_score) / temperature) >= r)) {
            best_solution = neighbour_solution;
            best_score = neighbour_score;
        }

    }

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;
    return best_solution;
}

vector<vector<list<int>>> Graph::getNeighbours(vector<list<int>> solution, int neighborhood_size, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&)) {
    vector<vector<list<int>>> array;

    for(int i = 0; i < neighborhood_size; i++) {
        array.push_back((this->*mutation_func)(solution));
    }

    return array;
}

template <class Type>
bool queueContainsElem(queue<Type> queue, Type element) {
    int iterations = queue.size();
    bool contains = false;

    for(int i = 0; i < iterations; i++) {
        vector<list<int>> elem = queue.front();
        queue.pop();

        if(elem == element)  {
            contains = true;
        }

        queue.push(elem);
    }

    return contains;
}

vector<list<int>> Graph::tabuSearch(int iteration_number, int tabu_size, int neighborhood_size, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&),
                                    int (Graph::*evaluation_func)(const vector<list<int>>&), bool log) {
    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    queue<vector<list<int>>> tabu_list;

    for(int i = 0; i < iteration_number; i++) {
        vector<vector<list<int>>> neighbourhood = this->getNeighbours(best_solution, neighborhood_size, (mutation_func));
        vector<list<int>> best_neighbour_solution;
        int best_neighbour_score = numeric_limits<int>::min();

        for(const auto& neighbour: neighbourhood) {
            int neighbour_score = (this->*evaluation_func)(neighbour);

            if ((neighbour_score > best_neighbour_score) && !queueContainsElem(tabu_list, neighbour) ) {
                best_neighbour_solution = neighbour;
                best_neighbour_score = neighbour_score;
            }
        }

        if(best_neighbour_score > best_score) {
            best_solution = best_neighbour_solution;
            best_score = best_neighbour_score;
        }
        tabu_list.push(best_neighbour_solution);

        if(tabu_list.size() > tabu_size) { tabu_list.pop(); }

    }

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;
    return best_solution;
}

bool Graph::check_solution(vector<list<int>> solution) {
    // check if hours of path doesnt exceed 8 hours
    for (auto &t: times) t = departure_time;
    for (int i = 0; i < solution.size(); i++) {
        for (auto it = solution[i].begin(); it != solution[i].end(); ++it) {
            if(it == solution[i].begin()) continue;
            else {
                int node1 = *prev(it);
                int node2 = *it;
                auto d = operationTime(node1, node2, times[i]);
                times[i].addTime(d);
            }
        }
        if(limit_time < times[i] ) return false;
    }
    return true;

}

void Graph::plotGraph() {
    // plot establishments (lat, long) in a map
    using namespace matplot;


    auto solution = generate_closest_solution();

    figure_handle f = figure(true);
    Geoplot_draw s(*this, f->current_axes());
    s.draw_all_vehicles(solution);


}

vector<list<int>> Graph::geneticAlgorithm(int iteration_number, int population_size, int tournament_size, int mutation_probability,
                                          vector<vector<list<int>>> (Graph::*crossover_func)(const vector<list<int>> &, const vector<list<int>> &),
                                          vector<list<int>> (Graph::*mutation_func)(const vector<list<int>> &),
                                          int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {

    vector<vector<list<int>>> population = this->generatePopulation(population_size);

    vector<list<int>> best_solution = population[0];
    int best_score = (this->*evaluation_func)(best_solution);
    int best_solution_generation = 0;

    printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    int generation_no = 0;

    while (iteration_number > 0) {

        generation_no += 1;

        vector<list<int>> tournament_winner = tournamentSelection(population, tournament_size, (evaluation_func));
        vector<list<int>> roulette_winner = rouletteSelection(population, (evaluation_func));

        vector<vector<list<int>>> crossovers = (this->*crossover_func)(tournament_winner, roulette_winner);
        vector<list<int>> crossover1 = crossovers[0];
        vector<list<int>> crossover2 = crossovers[1];

        int mutation_chance = engine() % 10;
        if(mutation_chance < mutation_probability) {
            crossover1 = (this->*mutation_func)(crossover1);
            crossover2 = (this->*mutation_func)(crossover2);
        }

        population = replace_least_fittest(population, crossover1, (evaluation_func));
        population = replace_least_fittest(population, crossover2, (evaluation_func));

        pair<vector<list<int>>, int> greatest_fit_and_score = get_greatest_fit(population, (evaluation_func));

        if(greatest_fit_and_score.second > best_score) {
            best_solution = greatest_fit_and_score.first;
            best_score = greatest_fit_and_score.second;
            best_solution_generation = generation_no;
            if (log) {
                cout << "\nGeneration: " << best_solution_generation << endl;
                cout << "Solution is valid: " << check_solution(best_solution) << endl;
                cout << "Score: " << best_score << endl;
            }
        } else iteration_number -= 1;
    }

    printSolution(best_solution);
    cout << "Best solution found in generation: " << best_solution_generation << endl;
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;
    return best_solution;
}

vector<vector<list<int>>> Graph::generatePopulation(int population_size) {
    vector<vector<list<int>>> population;
    for (int i = 0; i < population_size; i++) {
        population.push_back(this->generate_random_solution());
    }
    return population;
}

vector<list<int>> Graph::tournamentSelection(vector<vector<list<int>>> population, int size,
                                int (Graph::*evalFunction)(const vector<list<int>> &)) {
    if(size > population.size()) size = population.size();

    vector<list<int>> best_solution;
    int best_score;

    for (int i = 0; i < size; i++) {
        int random_index = engine() % population.size();
        vector<list<int>> solution = population[random_index];
        int score = (this->*evalFunction)(solution);

        if (i == 0) {
            best_score = score;
            best_solution = solution;
        } else if (score > best_score) {
            best_score = score;
            best_solution = solution;
        }
        population.erase(population.begin() + random_index);
    }

    return best_solution;
}

vector<list<int>> Graph::rouletteSelection(vector<vector<list<int>>> population, int (Graph::*evalFunction)(const vector<list<int>> &)) {

    vector<list<int>> best_solution = population[0];
    vector<int> scores;

    for (auto &solution: population) {
        scores.push_back((this->*evalFunction)(solution));
    }

    int total_score = 0;

    for (auto &score: scores) {
        total_score += score;
    }

    vector<double> probabilities;

    for (auto &score: scores) {
        probabilities.push_back((double) score / total_score);
    }

    // Choose random number based on probabilities
    double random_number = (double) engine() / engine.max();

    double sum = 0;
    for (int i = 0; i < probabilities.size(); i++) {
        sum += probabilities[i];
        if (sum >= random_number) {
            return population[i];
        }
    }
}

vector<vector<list<int>>> Graph::replace_least_fittest(vector<vector<list<int>>> population, vector<list<int>> new_solution,
                                                       int (Graph::*evalFunction)(const vector<list<int>> &)) {
    int least_fit_index = 0;
    int least_fit_score = (this->*evalFunction)(population[0]);

    for (int i=0; i<population.size(); i++) {
        int score = (this->*evalFunction)(population[i]);
        if (score < least_fit_score) {
            least_fit_score = score;
            least_fit_index = i;
        }
    }

    population[least_fit_index] = std::move(new_solution);

    return population;
}

pair<vector<list<int>>, int>
Graph::get_greatest_fit(vector<vector<list<int>>> population, int (Graph::*evalFunction)(const vector<list<int>>&)) {
    vector<list<int>> best_solution = population[0];
    int best_score = 0;

    for(auto &solution: population) {
        int score = (this->*evalFunction)(solution);
        if(score > best_score) {
            best_score = score;
            best_solution = solution;
        }
    }

    return make_pair(best_solution, best_score);
}

vector<vector<list<int>>> Graph::crossover_test(const vector<list<int>> &parent1, const vector<list<int>> &parent2) {
    vector<list<int>> child1 = parent1;
    vector<list<int>> child2 = parent2;

    return {child1, child2};
}


float Graph::Node::getLatitude() const {
    return latitude;
}

float Graph::Node::getLongitude() const {
    return longitude;
}

const string &Graph::Node::getAddress() const {
    return address;
}
