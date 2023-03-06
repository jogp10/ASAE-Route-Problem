#include "graph.h"


// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir) : n(num), hasDir(dir), nodes(num+1) {
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

void Graph::bfs(int a, int b) {
    for (int v=1; v<=n; v++) nodes[v].visited = false;
    queue<int> q; // queue of unvisited nodes

    q.push(a);
    nodes[a].dist=0;
    nodes[a].visited=true;

    while (!q.empty()) { // while there are still unvisited nodes

        int u = q.front(); q.pop();

        for (const auto& e : nodes[u].adj) {
            int w = e.dest;

            if (!nodes[w].visited) {
                q.push(w);
                nodes[w].pred = u;
                nodes[w].visited = true;
                nodes[w].dist = nodes[u].dist + e.weight;
            }
            if(w==b) break;
        }
    }

}

int Graph::evaluate_solution(const vector<list<int>>& solution) {
    int maxEstablishment = 0;
    int bestSolution = -1;
    for (int i = 0; i < solution.size(); ++i){
        if(solution[i].size() > maxEstablishment){
            maxEstablishment = solution[i].size();
            bestSolution = i;
        }
    }
    return bestSolution;
}

int Graph::random_node(list<Edge> &list) {
    int random = rand() % list.size();
    int i = 0;

    for (auto it = list.begin(); it != list.end(); ++it) {
        if (i == random) {
            return it->dest;
        }
        i++;
    }
    return -1;
}
