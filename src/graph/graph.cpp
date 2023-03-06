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
    list<int> unavailable;
    list<int> visited;
    int availables = nrVehicles;
    vector<list<int>> solution(nrVehicles, list<int>(1, 0));
    list<pair<int, int>> not_compatible_pairs;
    int max_tries = 1000;

    for (auto &t: times) t = departure_time;

    while (availables && visited.size() < n && max_tries) {
        int i = rand() % nrVehicles;
        int j = (rand() % n + 1) - 1;

        if (find(unavailable.begin(), unavailable.end(), i) != unavailable.end() || find(visited.begin(), visited.end(), j) != visited.end()) {
            cout << "Unavailable: " << i << " " << j << endl;
            max_tries--;
            continue;
        }

        if (find(not_compatible_pairs.begin(), not_compatible_pairs.end(), make_pair(i, j)) != not_compatible_pairs.end()) {
            cout << "Not compatible pair: " << i << " " << j << endl;
            max_tries--;
            continue;
        }

        int last = !solution[i].empty() ? solution[i].back() : 0;
        float time = this->getDistance(last, j);
        int seconds = (int) time;
        int miliseconds = (int) ((time - (float) seconds) * 1000);
        times[i].addTime({miliseconds, seconds, 0, 0});

        if (limit_time < times[i]) {
            times[i].subTime({miliseconds, seconds, 0, 0});
            not_compatible_pairs.emplace_back(i, j);
            continue;
        }

        int nr_hours = 0;
        bool stop = false;
        while (nodes[j].opening_hours[times[i].hours] == 0) {
            nr_hours++;
            times[i].toNextHour();
            if (limit_time < times[i]) {
                times[i].subTime({miliseconds, seconds, 0, 0});
                not_compatible_pairs.emplace_back(i, j);
                stop = true;

                for(int k = 0; k < nr_hours; ++k) times[i].toPreviousHour();
                break;
            }
        }
        if (stop) continue;

        visited.push_back(j);
        solution[i].push_back(j);
        times[i].addTime(0, 0, nodes[j].inspection_time, 0);
        if (limit_time < times[i]) {
            times[i].subTime({miliseconds, seconds, 0, 0});
            unavailable.push_back(i);
            availables--;
        }
    }

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

void Graph::Time::addTime(int milliseconds, int seconds, int minutes, int hours) {
    this->milliseconds += milliseconds;
    this->seconds += seconds;
    this->minutes += minutes;
    this->hours += hours;

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
        this->hours = this->hours % 24;
    }
}

void Graph::Time::setTime(int milliseconds, int seconds, int minutes, int hours) {
    this->milliseconds = milliseconds;
    this->seconds = seconds;
    this->minutes = minutes;
    this->hours = hours;
}

void Graph::Time::addTime(Graph::Time time) {
    this->addTime(time.milliseconds, time.seconds, time.minutes, time.hours);
}

bool Graph::Time::operator<(const Graph::Time &rhs) const {
    if (hours < rhs.hours) return true;
    if (rhs.hours < hours) return false;
    if (minutes < rhs.minutes) return true;
    if (rhs.minutes < minutes) return false;
    if (seconds < rhs.seconds) return true;
    if (rhs.seconds < seconds) return false;
    return milliseconds < rhs.milliseconds;
}

void Graph::Time::subTime(Graph::Time time) {
    this->subTime(time.milliseconds, time.seconds, time.minutes, time.hours);
}

void Graph::Time::subTime(int milliseconds, int seconds, int minutes, int hours) {
    this->milliseconds -= milliseconds;
    this->seconds -= seconds;
    this->minutes -= minutes;
    this->hours -= hours;

    if (this->milliseconds < 0)
    {
        this->seconds += this->milliseconds / 1000;
        this->milliseconds = this->milliseconds % 1000;
    }
    if (this->seconds < 0)
    {
        this->minutes += this->seconds / 60;
        this->seconds = this->seconds % 60;
    }

    if (this->minutes < 0)
    {
        this->hours += this->minutes / 60;
        this->minutes = this->minutes % 60;
    }

    if (this->hours < 0)
    {
        this->hours = this->hours % 24;
    }
}

void Graph::Time::toNextHour() {
    this->hours++;
    if (this->hours >= 24) {
        this->hours = 0;
    }
    this->minutes = 0;
    this->seconds = 0;
    this->milliseconds = 0;
}

void Graph::Time::toPreviousHour() {
    this->hours--;
    if (this->hours < 0) {
        this->hours = 23;
    }
    this->minutes = 0;
    this->seconds = 0;
    this->milliseconds = 0;

}
