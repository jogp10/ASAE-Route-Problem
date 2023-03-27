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
    u.dist = numeric_limits<float>::max();
    nodes[index] = u;
}


int Graph::evaluate_solution_1(const vector<list<int>>& solution) {
    int bestSolution = 0;
    for (const auto &i: solution)
        bestSolution +=  i.size();

    return bestSolution;
}

int Graph::evaluate_solution_2(const vector<std::list<int>> &solution) {
    int number_of_exchanges = 0;
    for(int i=0; i<solution.size(); i++) {
        auto before = next(solution[i].begin(), 1);
        auto it = next(before, 1);
        for(int j=2; j<solution.size()-1; j++) {
            if (nodes[*it].parish != "" and nodes[*before].parish != nodes[*it].parish) {
                number_of_exchanges += 1;
            }
        }
    }

    int size_solution = 0;
    for (int i=0; i<solution.size(); i++) size_solution += solution[i].size();

    return 100*size_solution -number_of_exchanges/size_solution;
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
    vector<MinHeap<int, float>> heaps(n, MinHeap<int, float>(n, -1));

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
    float min_dist = numeric_limits<float>::max();
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
    vector<float> closest_nodes_dist(order, numeric_limits<float>::max());

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
        for (int it : solution[i])  {
            vehicle_time += getDistance(last, it);
            last = it;
        }

        //cout << "Vehicle " << i << " total travel time: " << vehicle_time << "s" << endl;
        travel_time += vehicle_time;
    }
    //cout << "Total travel time: " << travel_time << "s" << endl;

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
            int seconds =  getDistance(last, *it);
            int miliseconds =  ((getDistance(last, *it) - (float) seconds) * 1000);
            time.addTime({miliseconds, seconds, 0, 0});

            auto aux = time;
            while (nodes[*it].opening_hours[time.hours] == 0) {
                time.toNextHour();
            }

            auto diff = time;
            diff.subTime(aux);
            vehicle_time += diff.toSeconds();

            time.addTime(0, 0, nodes[*it].inspection_time, 0);
            last = *it;
        }
        if(log) cout << "Vehicle " << i << " total waiting time: " << vehicle_time << "s" << endl;
        waiting_time += vehicle_time;
    }
    if(log) cout << "Total waiting time: " << waiting_time << "s" << endl;
    return waiting_time;
}


Time Graph::minimumOperationTime(int a, int b, Time time, bool log) {
    Time aux = time;

    float time_distance = getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    float time_depot = getDistance(b, 0);
    int milliseconds_distance = (int) ((getDistance(a, b) - (float) time_distance) * 1000);
    int milliseconds_depot = (int) ((getDistance(b, 0) - (float) time_depot) * 1000);

    Time w = {0, 0, 0, 0};
    w.addTime({milliseconds_distance,  (int)time_distance, 0, 0});
    if(log)  cout << "Time to travel to Node " << b << ": " << w.hours << ":" << w.minutes << "h" << endl;

    time.addTime({milliseconds_distance,  (int)time_distance, 0, 0});

    if(nodes[b].opening_hours[time.hours] == 0 && log) cout << "Node " << b << " is closed at " << time.hours << endl;
    for(int i = time.hours; i < limit_time.hours+1; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }
    if(log) cout << "Node " << b << " is open /(time) " << time.hours << ":" << time.minutes << "h" << endl;

    time.addTime({milliseconds_depot,  (int) time_depot, time_inspection, 0});
    if(log) cout << "Inspection time: " << time_inspection/60 << ":" << time_inspection%60 << endl;
    time.subTime(aux);
    return time;
}


Time Graph::operationTime(int a, int b, Time time, bool log) {
    Time aux = time;

    float time_distance = getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    int milliseconds_distance =  ((getDistance(a, b) - (float) time_distance) * 1000);

    time.addTime({milliseconds_distance,  (int)time_distance, 0, 0, 0});

    for(int i = 0; i < 24; i++) {
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
        //cout << "Vehicle " << i << " total operation time: " << vehicle_time << "s" << endl;
        operation_time += vehicle_time;
        if(vehicle_time > 28800) number_above++;
    }
    //cout << "Total operation time: " << operation_time << "s" << endl;
    //cout << "Number of vehicles above 8h: " << number_above << endl;
    return operation_time;
}


void Graph::printDetailedSolution(const vector<list<int>> &solution, bool log) {
    for (int i = 0; i < solution.size(); ++i) {
        Time t = departure_time;
        int last = 0;

        auto it = solution[i].begin();
        it++;
        for (; it != solution[i].end(); ++it)  {
            cout << "Vehicle " << i << " Going to Node " << *it << " " << t.hours << ":" << t.minutes << "h" << endl;
            cout << "Time to travel to Node " << *it << " " <<  getDistance(last, *it)/60 << "m" << endl;
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
    int node = (engine() % (new_solution[vehicle].size() - 2) + 1);
    int node2 = (engine() % (new_solution[vehicle2].size() - 2) + 1);
    while(vehicle == vehicle2 && node ==node2){
        node2 = (engine() % (new_solution[vehicle2].size() - 2) + 1);
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
    int vehicle =  engine() % nrVehicles;
    int node =  (engine() % (new_solution[vehicle].size() - 2) + 1);
    int node2 =  (engine() % (nodes.size() - 1) + 1);

    for (int i = 0; i < new_solution.size(); i++) {
        for (auto it = new_solution[i].begin(); it != new_solution[i].end(); ++it) {
            if (*it == node2) {
                node2 =  (engine() % (nodes.size() - 1) + 1);
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
    int node = (engine() % (new_solution[vehicle].size() - 2) + 1);
    int node2 =(engine() % (new_solution[vehicle].size() - 2) + 1);

    if(solution[vehicle].size() < 5) return solution;

    while(abs(node - node2) < 2) {
        node = (engine() % (new_solution[vehicle].size() - 2) + 1);
        node2 = (engine() % (new_solution[vehicle].size() - 2) + 1);
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
                node3 =  (engine() % (nodes.size() - 1) + 1);
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
    int mutation =  (engine() % 5 + 1);
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

int shortest_path_size(vector<list<int>> father_solution, vector<list<int>> mother_solution) {
    int shortest = INT32_MAX;

    for(int i = 0; i < father_solution.size(); i++) { // both solutions have the same size
        if(father_solution[i].size() < shortest) shortest =  father_solution[i].size();
        if(mother_solution[i].size() < shortest) shortest =  mother_solution[i].size();
    }

    return shortest;
}

vector<list<int>> Graph::fillSolution(const vector<list<int>> &child) {
    vector<MinHeap<int, float>> heaps(n, MinHeap<int, float>(n, -1));
    vector<list<int>> new_solution(nrVehicles);

    for (auto &n: nodes) {
        for(auto &e: n.adj) {
            heaps[n.id].insert(e.dest, e.weight);
        }
        n.visited = false;
    }

    for (auto &t: times) {
        t = departure_time;
    }

    for (int i=0; i<nrVehicles; i++) {
        auto before = child[i].begin();
        auto it = next(before, 1);
        new_solution[i].push_back(0);

        for (int j=1; j<child[i].size()-1; j++) {
            nodes[*it].visited = true;
            times[i].addTime(operationTime(*before, *it, times[i], false));
            new_solution[i].push_back(*it);

            before = it;
            it = next(it, 1);
        }
    }

    for(int i=0; i<nrVehicles; i++) {
        int limit = 3;

        for(int j=0; j<n-new_solution[i].size()-2; j++) {
            int last = new_solution[i].back();
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
            new_solution[i].push_back(nthClosest);
            Time op = operationTime(last, nthClosest, times[i], false);
            times[i].addTime(op);
            j=-1;
        }
    }

    for(int i=0; i<nrVehicles; i++) {
        new_solution[i].push_back(0);
    }

    return new_solution;
}

pair<vector<list<int>>, vector<list<int>>> Graph::crossover_solutions_1(const vector<list<int>> &father_solution, const vector<list<int>> &mother_solution) {
    int midpoint = engine() % (nrVehicles - 2) + 1;
    vector<list<int>> child1(father_solution.begin(), father_solution.begin()+midpoint), child2(mother_solution.begin(), mother_solution.begin()+midpoint);

    child1.insert(child1.end(), mother_solution.begin()+midpoint, mother_solution.end());
    child2.insert(child2.end(), father_solution.begin()+midpoint, father_solution.end());


    set<int> used_establishments1;

    for(auto van: child1) {
        auto it = van.begin();
        while(it != van.end()) {
            int previous_size = used_establishments1.size();
            used_establishments1.insert(*it);

            if(previous_size == used_establishments1.size()) {
                it = van.erase(it);
                continue;
            }
            it++;
        }
    }

    set<int> used_establishments2;

    for(auto van: child2) {
        auto it = van.begin();
        while(it != van.end()) {
            int previous_size = used_establishments2.size();
            used_establishments2.insert(*it);

            if(previous_size == used_establishments2.size()) {
                it = van.erase(it);
                continue;
            }
            it++;
        }
    }

    /*
    cout << "Father: " << this->evaluate_solution(father_solution) << endl;
    cout << "Mother: " << this->evaluate_solution(mother_solution) << endl;
    cout << "Child 1 unfilled: " << this->evaluate_solution(child1) << endl;
    cout << "Child 2 unfilled: " << this->evaluate_solution(child2) << endl;
    */

    //child1 = fillSolution(child1);
    //child2 = fillSolution(child2);

    /*
    cout << "Child 1 filled: " << this->evaluate_solution(child1) << endl;
    cout << "Child 2 filled: " << this->evaluate_solution(child2) << endl;
     */

    return make_pair(child1, child2);
}

pair<vector<list<int>>, vector<list<int>>> Graph::crossover_solutions_2(const vector<list<int>> &father_solution, const vector<list<int>> &mother_solution) {
    int midpoint1 = nrVehicles/3;
    int midpoint2 = 2*nrVehicles/3;

    vector<list<int>> child1(father_solution.begin(), father_solution.begin()+midpoint1), child2(mother_solution.begin(), mother_solution.begin()+midpoint1);

    child1.insert(child1.end(), mother_solution.begin()+midpoint1, mother_solution.begin()+midpoint2);
    child2.insert(child2.end(), father_solution.begin()+midpoint1, father_solution.begin()+midpoint2);

    child1.insert(child1.end(), father_solution.begin()+midpoint2, father_solution.end());
    child2.insert(child2.end(), mother_solution.begin()+midpoint2, mother_solution.end());

    set<int> used_establishments1;

    for(auto van: child1) {
        auto it = van.begin();
        while(it != van.end()) {
            int previous_size = used_establishments1.size();
            used_establishments1.insert(*it);

            if(previous_size == used_establishments1.size()) {
                it = van.erase(it);
                continue;
            }
            it++;
        }
    }

    set<int> used_establishments2;

    for(auto van: child2) {
        auto it = van.begin();
        while(it != van.end()) {
            int previous_size = used_establishments2.size();
            used_establishments2.insert(*it);

            if(previous_size == used_establishments2.size()) {
                it = van.erase(it);
                continue;
            }
            it++;
        }
    }

    return make_pair(child1, child2);
}

pair<vector<list<int>>, vector<list<int>>> Graph::crossover_solutions_3(const vector<list<int>> &father_solution, const vector<list<int>> &mother_solution) {
    int under5percentile = nrVehicles*0.05;
    int under10percentile = nrVehicles*0.1;

    auto father_ordered = father_solution;
    auto mother_ordered = mother_solution;

    sort(father_ordered.begin(), father_ordered.end(), [](const list<int> &a, const list<int> &b) {
        return a.size() < b.size();
    });

    sort(mother_ordered.begin(), mother_ordered.end(), [](const list<int> &a, const list<int> &b) {
        return a.size() < b.size();
    });

    vector<list<int>> child1(100), child2(100);

    auto it = child1.begin();
    auto it2 = child2.begin();

    for(int i = 0; i < under5percentile; i++) {
        *it = father_ordered[i];
        it++;
        *it2 = mother_ordered[i];
        it2++;
    }

    for(int i = under5percentile; i < under10percentile; i++) {
        *it = mother_ordered[i];
        it++;
        *it2 = father_ordered[i];
        it2++;
    }

    // remove possible repeated nodes
    set<int> used_establishments1;

    for(auto van: child1) {
        auto it = van.begin();
        while(it != van.end()) {
            int previous_size = used_establishments1.size();
            used_establishments1.insert(*it);

            if(previous_size == used_establishments1.size()) {
                it = van.erase(it);
                continue;
            }
            it++;
        }
    }

    fillSolution(child1);
    fillSolution(child2);

    return make_pair(child1, child2);
}

vector<list<int>> Graph::hillClimbing(const int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {
    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);

    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;


    for(int i=0; i<iteration_number; i++) {
        vector<list<int>> neighbour_solution = (this->*mutation_func)(best_solution);
        int neighbour_score = (this->*evaluation_func)(neighbour_solution);

        if(neighbour_score > best_score) {
            best_solution = neighbour_solution;
            best_score = neighbour_score;

            if (log) {
                cout << "Iteration: " << i << endl;
                cout << "Score: " << best_score << endl;
            }
        }
    }

    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;
    return best_solution;
}


vector<list<int>> Graph::simulatedAnnealing(const int iteration_number, const float coldness_ratio, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {
    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);

    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    float temperature = 1000;

    for(int i=0; i<iteration_number; i++) {
        temperature *= coldness_ratio;

        vector<list<int>> neighbour_solution = (this->*mutation_func)(best_solution);
        int neighbour_score = (this->*evaluation_func)(neighbour_solution);
        float r = static_cast <float> (engine()) / static_cast <float> (RAND_MAX);

        if((neighbour_score > best_score) or (pow(M_E, (float)(neighbour_score - best_score) / temperature) >= r)) {
            best_solution = neighbour_solution;
            best_score = neighbour_score;

            if (log) {
                cout << "Iteration: " << i << endl;
                cout << "Score: " << best_score << endl;
            }
        }
    }

    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;
    return best_solution;
}

vector<vector<list<int>>> Graph::getNeighbours(const vector<list<int>>& solution, int neighborhood_size, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&)) {
    vector<vector<list<int>>> array;

    for(int i = 0; i < neighborhood_size; i++) {
        array.push_back((this->*mutation_func)(solution));
    }

    return array;
}

bool queueContainsElem(queue<int> queue, int element) {
    int iterations = queue.size();
    bool contains = false;

    for(int i = 0; i < iterations; i++) {
        int elem = queue.front();
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
    queue<int> tabu_list;

    //printSolution(best_solution);
    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    for(int i = 0; i < iteration_number; i++) {
        vector<vector<list<int>>> neighbourhood = this->getNeighbours(best_solution, neighborhood_size, (mutation_func));
        vector<list<int>> best_neighbour_solution;
        int best_neighbour_score = numeric_limits<int>::min();

        for(const auto& neighbour: neighbourhood) {
            int neighbour_score = (this->*evaluation_func)(neighbour);
            int total_travel_time = totalTravelTime(neighbour);

            if ((neighbour_score > best_neighbour_score) && !queueContainsElem(tabu_list, total_travel_time) ) {
                best_neighbour_solution = neighbour;
                best_neighbour_score = neighbour_score;
            }
        }

        if(best_neighbour_score > best_score) {
            best_solution = best_neighbour_solution;
            best_score = best_neighbour_score;

            if (log) {
                cout << "Iteration: " << i << endl;
                cout << "Score: " << best_score << endl;
            }
        }

        //add solution to tabu list
        int ttt_best_neighbour = totalTravelTime(best_neighbour_solution);
        tabu_list.push(ttt_best_neighbour);

        if(tabu_list.size() > tabu_size) { tabu_list.pop(); }

    }

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
                                          pair<vector<list<int>>, vector<list<int>>> (Graph::*crossover_func)(const vector<list<int>>&, const vector<list<int>>&),
                                          vector<list<int>> (Graph::*mutation_func)(const vector<list<int>> &),
                                          int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {

    vector<vector<list<int>>> population = this->generatePopulation(population_size);
    iterations.clear();
    vector<list<int>> best_solution = population[0];
    int best_score = (this->*evaluation_func)(best_solution);
    int best_solution_generation = 0;

    cout << "Solution is valid: " << check_solution(best_solution) << endl;
    cout << "Score: " << best_score << endl;

    int generation_no = 0;

    while (iteration_number > 0) {

        generation_no += 1;

        vector<list<int>> tournament_winner = tournamentSelection(population, tournament_size, (evaluation_func));
        vector<list<int>> roulette_winner = rouletteSelection(population, (evaluation_func));

        pair<vector<list<int>>, vector<list<int>>> crossovers = (this->*crossover_func)(tournament_winner, roulette_winner);
        vector<list<int>> crossover1 = crossovers.first;
        vector<list<int>> crossover2 = crossovers.second;

        int mutation_chance = engine() % 101;
        if(mutation_chance < mutation_probability) {
            crossover1 = (this->*mutation_func)(crossover1);
            crossover2 = (this->*mutation_func)(crossover2);
        }

        population = replace_least_fittest(population, crossover1, (evaluation_func));
        population = replace_least_fittest(population, crossover2, (evaluation_func));

        pair<vector<list<int>>, int> greatest_fit_and_score = get_greatest_fit(population, (evaluation_func));

        iterations.push_back(greatest_fit_and_score.second);

        if(greatest_fit_and_score.second > best_score) {
            best_solution = greatest_fit_and_score.first;
            best_score = greatest_fit_and_score.second;
            best_solution_generation = generation_no;
            if (log) {
                cout << "\nGeneration: " << best_solution_generation << endl;
                cout << "Solution is valid: " << check_solution(best_solution) << endl;
                cout << "Score: " << best_score << endl;

            }
        } iteration_number -= 1;

    }

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
    if(size > population.size()) size =  population.size();

    vector<list<int>> best_solution;
    int best_score;

    for (int i = 0; i < size; i++) {
        int random_index =  engine() % population.size();
        vector<list<int>> solution = population[random_index];
        int score = (this->*evalFunction)(solution);

        if (i == 0) {
            best_score = score;
            best_solution = solution;
        }
        if (score > best_score) {
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

    int total_score = numeric_limits<float>::min();

    for (auto &score: scores) {
        total_score += score;
    }

    vector<double> probabilities;

    for (auto &score: scores) {
        probabilities.push_back((double) score / total_score);
    }

    // Choose random number based on probabilities
    double random_number = (double) engine() / std::mt19937::max();

    double sum = 0;
    for (int i = 0; i < probabilities.size(); i++) {
        sum += probabilities[i];
        if (sum >= random_number) {
            return population[i];
        }
    }
    return {};
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

pair<vector<list<int>>, int> Graph::get_greatest_fit(vector<vector<list<int>>> population,
                                                     int (Graph::*evalFunction)(const vector<list<int>>&)) {
    vector<list<int>> best_solution = population[0];
    int best_score = numeric_limits<float>::min();

    for(auto &solution: population) {
        int score = (this->*evalFunction)(solution);
        if(score > best_score) {
            best_score = score;
            best_solution = solution;
        }
    }

    return make_pair(best_solution, best_score);
}

std::vector<std::list<int>> Graph::generate_a_star_solution(bool log) {
    vector<list<int>> solution(nrVehicles, list<int>(1, 0));
    vector<MinHeap<int, float>> heaps(n, MinHeap<int, float>(n, -1));
    vector<int> number_of_open_hours(n, 0);

    for(int i=0; i<n; i++) {
        for(int j=0; j<24; j++) {
            if(nodes[i].opening_hours[j]) number_of_open_hours[i]++;
        }
    }


    for (auto &t: times) t = departure_time;
    for (auto &n: nodes) {
        for(auto &e: n.adj) {
            heaps[n.id].insert(e.dest, e.weight - number_of_open_hours[n.id]);
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

const vector<int> &Graph::getIterations() const {
    return iterations;
}

void Graph::evolutionGraph(std::vector<int> iterations, string title) {
    using namespace matplot;

    figure_handle f = figure(true);
    Geoplot_draw s(*this, f->current_axes());

    s.evolution_graph(std::move(iterations), title);

    //plot(iterations);
    //title(ax1, "Top Plot");
    //ylabel(ax1, "sin(5x)");




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
