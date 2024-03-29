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
    nodes[index] = u;
}


int Graph::evaluate_solution_1(const vector<list<int>>& solution) {
    int bestSolution = 0;
    for (const auto &i: solution)
        bestSolution +=  i.size() - 2;

    return bestSolution;
}


int Graph::evaluate_solution_2(const vector<std::list<int>> &solution) {
    int number_of_exchanges = 0;
    for(const auto & i : solution) {
        auto before = next(i.begin(), 1);
        auto it = next(before, 1);
        for(int j=2; j<i.size()-1; j++) {
            if (!nodes[*it].parish.empty() and nodes[*before].parish != nodes[*it].parish) {
                number_of_exchanges += 1;
            }
            before = it;
            it = next(it, 1);
        }
    }

    int size_solution = 0;
    for (const auto & i : solution) size_solution += i.size() - 2;
    if (size_solution == 0) return 0;
    return 100*size_solution + (100 - ((float) number_of_exchanges/size_solution)*100);
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


        Time min_op = minimumOperationTime(last, j, times[i], log);
        min_op.addTime(times[i]);
        if (limit_time < min_op) {
            not_compatible_pairs[i].emplace_back(j);
            continue;
        }

        nr_visited++;
        nodes[j].visited = true;
        solution[i].push_back(j);
        Time op = operationTime(last, j, times[i], log);
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


            Time min_op = minimumOperationTime(last, nthClosest, times[i], log);

            min_op.addTime(times[i]);
            if (limit_time < min_op) {
                limit--;
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

std::list<int> Graph::getVehiclePath(int vehicle, const vector<std::list<int>> &solution) {
    if (vehicle < solution.size()) {
        return solution[vehicle];
    }
    return {};
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

        if (log)
            cout << "Vehicle " << i << " total travel time: " << vehicle_time << "s" << endl;
        travel_time += vehicle_time;
    }
    if (log)
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
            int seconds =  getDistance(last, *it);
            int milliseconds =  ((getDistance(last, *it) - (float) seconds) * 1000);
            time.addTime({milliseconds, seconds, 0, 0});

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

    time.addTime({milliseconds_distance,  (int)time_distance, 0, 0});

    for(int i = time.hours; i < limit_time.hours+1; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }

    time.addTime({milliseconds_depot,  (int) time_depot, time_inspection, 0});
    time.subTime(aux);
    return time;
}


Time Graph::operationTime(int a, int b, Time time, bool log) {
    Time aux = time;

    float time_distance = getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    int milliseconds_distance =  ((getDistance(a, b) - (float) time_distance) * 1000);

    if(log)  cout << "Time to travel to Node " << b << ": " << time.hours << ":" << time.minutes << "h" << endl;

    time.addTime({milliseconds_distance,  (int)time_distance, 0, 0, 0});

    if(nodes[b].opening_hours[time.hours] == 0 && log) cout << "Node " << b << " is closed at " << time.hours << endl;
    for(int i = 0; i < 24; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }
    if(log) cout << "Node " << b << " is open /(time) " << time.hours << ":" << time.minutes << "h" << endl;

    if(b!=0)time.addTime({0, 0, time_inspection, 0, 0});

    if(log) cout << "Inspection time: " << time_inspection/60 << ":" << time_inspection%60 << endl;

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
            t.addTime(operationTime(last, *it, t, log));
            last = *it;
        }

        t.subTime(departure_time);
        float vehicle_time = t.toSeconds();
        if (log)
            cout << "Vehicle " << i << " total operation time: " << vehicle_time << "s" << endl;
        operation_time += vehicle_time;
        if(vehicle_time > 28800) number_above++;
    }
    if (log) {
        cout << "Total operation time: " << operation_time << "s" << endl;
        cout << "Number of vehicles above 8h: " << number_above << endl;
    }
    return operation_time;
}


void Graph::printDetailedSolution(const vector<list<int>> &solution) {
    for (const auto & i : solution) {
        Time t = departure_time;
        int last = 0;

        auto it = i.begin();
        it++;
        for (; it != i.end(); ++it)  {
            t.addTime(operationTime(last, *it, t, true));
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
    while(vehicle == vehicle2 && node ==node2 && solution[vehicle].size() > 3){
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
    auto s = solution;
    switch (mutation) {
        case 1:
            s = mutation_solution_1(solution);
            return mutation_solution_5(s);
        case 2:
            s = mutation_solution_2(solution);
            return mutation_solution_5(s);
        case 3:
            s = mutation_solution_3(solution);
            return mutation_solution_5(s);
        case 4:
            s = mutation_solution_4(solution);
            return mutation_solution_5(s);
        case 5:
            return mutation_solution_5(solution);
        default:
            return solution;
    }
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
            times[i].addTime(operationTime(*before, *it, times[i]));
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

            Time min_op = minimumOperationTime(last, nthClosest, times[i]);

            min_op.addTime(times[i]);
            if (limit_time < min_op) {
                limit--;
                continue;
            }

            nodes[nthClosest].visited = true;
            new_solution[i].push_back(nthClosest);
            Time op = operationTime(last, nthClosest, times[i]);
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
    if(nrVehicles < 2) {
        return make_pair(father_solution, mother_solution);
    }

    int midpoint = engine() % nrVehicles + 1;
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

    return make_pair(child1, child2);
}


pair<vector<list<int>>, vector<list<int>>> Graph::crossover_solutions_2(const vector<list<int>> &father_solution, const vector<list<int>> &mother_solution) {
    if(nrVehicles < 3) {
        return make_pair(father_solution, mother_solution);
    }

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


bool Graph::check_solution(vector<list<int>> solution) {
    // check if hours of path don't exceed 8 hours
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


vector<list<int>> Graph::hillClimbing(const int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &), bool log, const vector<list<int>>& initial_solution) {
    startTimer();

    vector<list<int>> best_solution;
    initial_solution.empty() ? best_solution = this->generate_closest_solution() : best_solution = initial_solution;
    int best_score = (this->*evaluation_func)(best_solution);

    cout << "Initial Score: " << best_score << endl;

    if(initial_solution.empty()) iterations = {};

    for(int i=0; i<iteration_number; i++) {
        vector<list<int>> neighbour_solution = (this->*mutation_func)(best_solution);
        int neighbour_score = (this->*evaluation_func)(neighbour_solution);

        if(neighbour_score > best_score) {
            best_solution = neighbour_solution;
            best_score = neighbour_score;
            updateTimer(i);

            if (log) {
                cout << "Iteration: " << i;
                cout << ", New Score: " << best_score << endl;
            }
        }
        iterations.push_back(best_score);
    }

    endTimer();

    cout << "Final Score: " << best_score << endl;
    last_solution = best_solution;
    return best_solution;
}


vector<list<int>> Graph::simulatedAnnealing(const int iteration_number, const float coldness_ratio, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {
    startTimer();

    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);
    cout << "Initial Score: " << best_score << endl;

    float temperature = 1000;

    iterations = {};

    for(int i=0; i<iteration_number; i++) {
        temperature *= coldness_ratio;

        vector<list<int>> neighbour_solution = (this->*mutation_func)(best_solution);
        int neighbour_score = (this->*evaluation_func)(neighbour_solution);
        float r = static_cast <float> (engine()) / static_cast <float> (RAND_MAX);

        iterations.push_back(neighbour_score);
        if((neighbour_score > best_score) or (pow(M_E, (float)(neighbour_score - best_score) / temperature) >= r)) {
            best_solution = neighbour_solution;
            best_score = neighbour_score;
            updateTimer(i);

            if (log) {
                cout << "Iteration: " << i;
                cout << ", New Score: " << best_score << endl;
            }
        }
    }

    endTimer();

    cout << "Final Score: " << best_score << endl;
    last_solution = best_solution;
    return best_solution;
}


vector<list<int>> Graph::tabuSearch(int iteration_number, int tabu_size, int neighborhood_size, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&),
                                    int (Graph::*evaluation_func)(const vector<list<int>>&), bool log) {
    startTimer();

    vector<list<int>> best_solution = this->generate_closest_solution();
    int best_score = (this->*evaluation_func)(best_solution);
    queue<int> tabu_list;

    cout << "Initial Score: " << best_score << endl;

    iterations = {};
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
            updateTimer(i);

            if (log) {
                cout << "Iteration: " << i;
                cout << ", New Score: " << best_score << endl;
            }
        }

        iterations.push_back(best_score);
        //add a solution to tabu list
        int ttt_best_neighbour = totalTravelTime(best_neighbour_solution);
        tabu_list.push(ttt_best_neighbour);

        if(tabu_list.size() > tabu_size) { tabu_list.pop(); }
    }

    endTimer();

    cout << "Final Score: " << best_score << endl;
    last_solution = best_solution;
    return best_solution;
}


vector<vector<list<int>>> Graph::getNeighbours(const vector<list<int>>& solution, int neighborhood_size, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&)) {
    vector<vector<list<int>>> array;

    array.reserve(neighborhood_size);
    for(int i = 0; i < neighborhood_size; i++) {
        array.push_back((this->*mutation_func)(solution));
    }

    return array;
}


bool Graph::queueContainsElem(queue<int> queue, int element) {
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


vector<list<int>> Graph::geneticAlgorithm(int iteration_number, int population_size, int tournament_size, int mutation_probability,
                                          pair<vector<list<int>>, vector<list<int>>> (Graph::*crossover_func)(const vector<list<int>>&, const vector<list<int>>&),
                                          vector<list<int>> (Graph::*mutation_func)(const vector<list<int>> &),
                                          int (Graph::*evaluation_func)(const vector<list<int>> &), bool log) {
    startTimer();

    vector<vector<list<int>>> population = this->generatePopulation(population_size);
    iterations = {};
    vector<list<int>> best_solution = population[0];
    int best_score = (this->*evaluation_func)(best_solution);
    int best_solution_generation = 0;

    cout << "One of initial Scores: " << best_score << endl;

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


        if(greatest_fit_and_score.second > best_score) {
            best_solution = greatest_fit_and_score.first;
            best_score = greatest_fit_and_score.second;
            best_solution_generation = generation_no;
            updateTimer(generation_no);

            if (log) {
                cout << "\nGeneration: " << best_solution_generation << endl;
                cout << "New Score: " << best_score << endl;

            }
        } iteration_number -= 1;
        iterations.push_back(best_score);

    }

    endTimer();

    cout << "Best solution found in generation: " << best_solution_generation << endl;
    cout << "Final Score: " << best_score << endl;
    last_solution = best_solution;
    return best_solution;
}


vector<vector<list<int>>> Graph::generatePopulation(int population_size) {
    vector<vector<list<int>>> population;
    auto n = this->generate_closest_solution();

    population.reserve(population_size);
    for (int i = 0; i < population_size; i++) {
        population.push_back(mutation_solution_6(n));
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

    scores.reserve(population.size());
    for (auto &solution: population) {
        scores.push_back((this->*evalFunction)(solution));
    }

    int total_score = numeric_limits<float>::min();

    for (auto &score: scores) {
        total_score += score;
    }

    vector<double> probabilities;

    probabilities.reserve(scores.size());
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

std::vector<std::list<int>> Graph::iteratedLocalSearch(int iteration_number, vector<list<int>> (Graph::*mutation_func)(const vector<list<int>>&), int (Graph::*evaluation_func)(const vector<list<int>> &)) {
    iterations = {};
    startTimer();
    auto st = start_time, ot = start_time;

    vector<list<int>> solution = generate_closest_solution(false);

    for(int i=0; i<iteration_number; i++) {
        cout << "Iteration " << i << endl;
        vector<list<int>> new_solution = hillClimbing(1000, mutation_func, evaluation_func, false, solution);
        if (evaluate_solution_2(new_solution) > evaluate_solution_2(solution)) {
            solution = new_solution;
            last_solution = solution;
            updateTimer(i);
            ot = optimal_time;
        }
        cout << endl;
    }

    endTimer();
    auto et = end_time;
    start_time = st;
    optimal_time = ot;
    return solution;
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


            Time min_op = minimumOperationTime(last, nthClosest, times[i], log);

            min_op.addTime(times[i]);
            if (limit_time < min_op) {
                limit--;
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


void Graph::plot_initial_solution(const vector<list<int>>& solution, string title) {
    // plot establishments (lat, long) in a map
    using namespace matplot;

    figure_handle f = figure(true);
    Geoplot_draw s(*this, f->current_axes());
    s.draw_all_vehicles(solution, std::move(title));
}


void Graph::evolutionGraph(std::vector<int> iterations, string title) {
    using namespace matplot;

    figure_handle f = figure(true);
    Geoplot_draw s(*this, f->current_axes());

    s.evolution_graph(std::move(iterations), std::move(title));
}


void Graph::compare_algorithms(std::vector<int> sol1, std::vector<int> sol2,
                               std::vector<int> sol3, std::vector<int> sol4, int num_iterations) {
    using namespace matplot;

        figure_handle f = figure(true);
        Geoplot_draw s(*this,f->current_axes());

        s.compare_algorithms(std::move(sol1), std::move(sol2),std::move(sol3),std::move(sol4),num_iterations);
}

const vector<int> &Graph::getIterations() const { return iterations; }

int Graph::getIterationsOptimal() const { return iterations_optimal; }

float Graph::getRuntimeOptimal() { return std::chrono::duration_cast<std::chrono::milliseconds>(optimal_time - start_time).count(); }

float Graph::getRuntime() { return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count(); }

void Graph::startTimer() { start_time = std::chrono::high_resolution_clock::now(); }

void Graph::endTimer() { end_time = std::chrono::high_resolution_clock::now(); }

void Graph::updateTimer(int i) {
    iterations_optimal = i+1;
    optimal_time = std::chrono::high_resolution_clock::now();
}

void Graph::plot_vehicle_from_solution(std::vector<std::list<int>> vector1, int i) {
    using namespace matplot;

    figure_handle f = figure(true);
    Geoplot_draw s(*this, f->current_axes());
    if (i == -1) {
        s.draw_all_vehicles(vector1);
        return;
    }
    else if (i < 0 || i >= vector1.size()
                || vector1[i].empty())
        return;

    s.draw_one_vehicle(getVehiclePath(i, vector1));
}

std::vector<std::list<int>> Graph::getLastSolution() { return last_solution; }

int Graph::getMaxVehicles() const { return nrVehicles; }

int Graph::get_Current_Establishments() const {
    return n;
}

float Graph::Node::getLatitude() const { return latitude; }

float Graph::Node::getLongitude() const { return longitude; }

const string &Graph::Node::getAddress() const { return address; }