#include "graph.h"


// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir, Time departure_time, Time max_work_time) : n(num), hasDir(dir), nodes(num+1),
                                                                           departure_time(departure_time), max_work_time(max_work_time) {
    this->time = this->departure_time;
    this->limit_time = this->departure_time;
    this->limit_time.addTime(this->max_work_time);
    this->setNrVehicles(n);
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
    u.address = address;
    u.latitude = latitude;
    u.longitude = longitude;
    u.inspection_time = inspection_time;
    u.inspection_utility = inspection_utility;
    u.opening_hours = opening_hours;
    u.dist = INT_MAX;
    nodes[index] = u;
}

int Graph::evaluate_solution(const vector<list<int>>& solution) {
    int bestSolution = 0;
    for (int i = 0; i < solution.size(); i++){
        bestSolution += solution[i].size();
    }
    return bestSolution;
}

int Graph::random_node(const int idx) {
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

vector<list<int>> Graph::generate_random_solution() const {
    vector<bool> available(n, true);

    return vector<list<int>>();
}

void Graph::showAllEstablishments() {
    for (int i = 1; i < nodes.size(); ++i) {
        cout << "Node " << i << ": " << nodes[i].address << endl;
    }
}

void Graph::Time::addTime(int seconds, int minutes, int hours) {
    this->seconds += seconds;
    this->hours += minutes;
    this->hours += hours;

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

void Graph::Time::setTime(int seconds, int minutes, int hours) {
    this->seconds = seconds;
    this->minutes = minutes;
    this->hours = hours;
}

void Graph::Time::addTime(Graph::Time time) {
    this->addTime(time.seconds, time.minutes, time.hours);
}
