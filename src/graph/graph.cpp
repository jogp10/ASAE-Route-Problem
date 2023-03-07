#include "graph.h"

#include <utility>
#include <list>
#include <algorithm>


// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir, Time departure_time, Time max_work_time) : n(num), hasDir(dir), nodes(num) {
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

void Graph::setNode(int index, string address, float latitude, float longitude, float inspection_utility, int inspection_time, vector<int> opening_hours) {
    Node u;
    u.id = index;
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

void Graph::hillClimbing() {

}

void Graph::simulatedAnnealing() {

}

void Graph::tabuSearch() {

}

void Graph::geneticAlgorithm() {

}

vector<list<int>> Graph::generate_random_solution() {

    int nr_visited = 0;
    vector<list<int>> not_compatible_pairs(nrVehicles, list<int>());
    vector<list<int>> solution(nrVehicles, list<int>(1, 0));
    int max_tries = n*10;

    for (auto &t: times) t = departure_time;
    for (auto &n: nodes) n.visited = false;

    while (nr_visited < n && max_tries) {
        int i = rand() % nrVehicles;
        int j = (rand() % (n - 1)) + 1;

        if (nodes[j].visited) {
            max_tries--;
            continue;
        }

        if (find(not_compatible_pairs[i].begin(), not_compatible_pairs[i].end(), j) != not_compatible_pairs[i].end()) {
            max_tries--;
            continue;
        }

        int last = solution[i].back();

        cout << "Vehicle -> " << i << "    Node -> " << last << endl;
        cout << times[i].hours << ":" << times[i].minutes << "h" << endl;

        Time min_op = minimumOperationTime(last, j, times[i]);
        min_op.addTime(times[i]);
        if (limit_time < min_op) {
            cout << "Limit time reached" << endl;
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

int Graph::closest_node(int idx) const {
    int closest = -1;
    float min_dist = INT_MAX;
    for(auto e: nodes[idx].adj){
        if(e.weight < min_dist){
            min_dist = e.weight;
            closest = e.dest;
        }
    }
    return closest;
}

float Graph::getDistance(int a, int b) {
    for (auto e: nodes[a].adj) {
        if (e.dest == b) {
            return e.weight;
        }
    }
}

float Graph::totalTravelTime(const vector<list<int>> &solution) {
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

float Graph::totalWaitingTime(const vector<list<int>> &solution) {
    float waiting_time = 0;

    for (int i = 0; i < solution.size(); ++i) {
        float vehicle_time = 0;
        Graph::Time time = departure_time;
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

Graph::Time Graph::minimumOperationTime(int a, int b, Time time) {
    Graph::Time aux = time;

    float time_distance = (int) getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    float time_depot = (int) getDistance(b, 0);
    int milliseconds_distance = (int) ((getDistance(a, b) - (float) time_distance) * 1000);
    int milliseconds_depot = (int) ((getDistance(b, 0) - (float) time_depot) * 1000);

    Time w = {0, 0, 0, 0};
    w.addTime({milliseconds_distance, (int) time_distance, 0, 0});
    cout << "Time to travel to Node " << b << ": " << w.hours << ":" << w.minutes << "h" << endl;

    time.addTime({milliseconds_distance, (int) time_distance, 0, 0});

    if(nodes[b].opening_hours[time.hours] == 0) cout << "Node " << b << " is closed at " << time.hours << endl;
    for(int i = time.hours; i < 24; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }
    cout << "Node " << b << " is open /(time) " << time.hours << ":" << time.minutes << "h" << endl;

    time.addTime({milliseconds_depot, (int) time_depot, time_inspection, 0});
    cout << "Inspection time: " << time_inspection/60 << ":" << time_inspection%60 << endl;
    time.subTime(aux);
    return time;
}

Graph::Time Graph::operationTime(int a, int b, Time time) {
    Graph::Time aux = time;

    float time_distance = (int) getDistance(a, b);
    int time_inspection = b != 0 ? nodes[b].inspection_time: 0;
    int milliseconds_distance = (int) ((getDistance(a, b) - (float) time_distance) * 1000);

    time.addTime({milliseconds_distance, (int) time_distance, 0, 0});

    for(int i = time.hours; i < 24; i++) {
        if(nodes[b].opening_hours[time.hours] == 0) {
            time.toNextHour();
        }
        else break;
    }

    time.addTime({0, 0, time_inspection, 0});
    time.subTime(aux);
    return time;
}

float Graph::totalOperationTime(const vector<list<int>> &solution) {
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

void Graph::printDetailedSolution(const vector<list<int>> &solution) {
    // For each step in vechicle i, node,time before leaving to node, time of travel distance to node, inspection time, time after inspection
    for (int i = 0; i < solution.size(); ++i) {
        Time t = departure_time;
        int last = 0;

        auto it = solution[i].begin();
        it++;
        for (; it != solution[i].end(); ++it)  {
            cout << "Time to travel to Node " << *it << " " << (int) getDistance(last, *it)/60 << "m" << endl;
            cout << "Vehicle " << i << " Node " << *it << " " << t.hours << ":" << t.minutes << "h" << endl;
            cout << "Time to inspect Node " << *it << ": " << nodes[*it].inspection_time/60 << ":" << nodes[*it].inspection_time%60 << endl << endl;
            t.addTime(operationTime(last, *it, t));
            last = *it;
        }
    }

}

void Graph::Time::addTime(int milliseconds, int seconds, int minutes, int hours, int days) {
    this->milliseconds += milliseconds;
    this->seconds += seconds;
    this->minutes += minutes;
    this->hours += hours;
    this->days += days;

    if (this->milliseconds >= 1000)
    {
        this->seconds += this->milliseconds / 1000;
        this->milliseconds = this->milliseconds % 1000;
    }
    if (this->seconds >= 60)
    {
        this->minutes += this->seconds / 60;
        this->seconds = this->seconds % 60;
    }

    if (this->minutes >= 60)
    {
        this->hours += this->minutes / 60;
        this->minutes = this->minutes % 60;
    }

    if (this->hours >= 24)
    {
        this->days += this->hours / 24;
        this->hours = this->hours % 24;
    }
}

void Graph::Time::setTime(int milliseconds, int seconds, int minutes, int hours, int days) {
    this->milliseconds = milliseconds;
    this->seconds = seconds;
    this->minutes = minutes;
    this->hours = hours;
    this->days = days;
}

void Graph::Time::addTime(Graph::Time time) {
    this->addTime(time.milliseconds, time.seconds, time.minutes, time.hours, time.days);
}

bool Graph::Time::operator<(const Graph::Time &rhs) const {
    if (days < rhs.days) return true;
    if (rhs.days < days) return false;
    if (hours < rhs.hours) return true;
    if (rhs.hours < hours) return false;
    if (minutes < rhs.minutes) return true;
    if (rhs.minutes < minutes) return false;
    if (seconds < rhs.seconds) return true;
    if (rhs.seconds < seconds) return false;
    return milliseconds < rhs.milliseconds;
}

void Graph::Time::subTime(Graph::Time time) {
    this->subTime(time.milliseconds, time.seconds, time.minutes, time.hours, time.days);
}

void Graph::Time::subTime(int milliseconds, int seconds, int minutes, int hours, int days) {
    Time aux = {milliseconds, seconds, minutes, hours, days};
    float aux_seconds = aux.toSeconds();
    float this_seconds = this->toSeconds();

    this_seconds -= aux_seconds;

    this->days = (int) this_seconds / 86400;
    this_seconds -= this->days * 86400;
    this->hours = (int) this_seconds / 3600;
    this_seconds -= this->hours * 3600;
    this->minutes = (int) this_seconds / 60;
    this_seconds -= this->minutes * 60;
    this->seconds = (int) this_seconds;
    this_seconds -= this->seconds;
    this->milliseconds = (int) (this_seconds * 1000);
}

void Graph::Time::toNextHour() {
    this->hours++;
    if (this->hours >= 24) {
        this->hours = 0;
        this->days++;
    }
    this->minutes = 0;
    this->seconds = 0;
    this->milliseconds = 0;
}

void Graph::Time::toPreviousHour() {
    this->hours--;
    if (this->hours < 0) {
        this->hours = 23;
        this->days--;
    }
    this->minutes = 0;
    this->seconds = 0;
    this->milliseconds = 0;
}

float Graph::Time::toSeconds() {
    return (float) this->milliseconds / 1000 + this->seconds + this->minutes * 60 + this->hours * 3600 + this->days * 86400;
}
