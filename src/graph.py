class graph:
    def __init__(self, directed=False):
        self.graph = {}
        self.n = 0
        self.directed = directed

    def add_node(self, u):
        if u not in self.graph:
            self.n += 1
            self.graph[u] = []

    def add_edge(self, u, v, w):
        if u not in self.graph:
            self.n += 1
            self.graph[u] = []
        self.graph[u].append((v, w))
        if not self.directed:
            if v not in self.graph:
                self.n += 1
                self.graph[v] = []
            self.graph[v].append((u, w))

    def remove_edge(self, u, v):
        for index, node in enumerate(self.graph[u]):
            if node[0] == v:
                self.graph[u].pop(index)
                break
        if not self.directed:
            for index, node in enumerate(self.graph[v]):
                if node[0] == u:
                    self.graph[v].pop(index)
                    break

    def nodes(self):
        return self.n

    def print(self):
        for node in self.graph:
            print(node, self.graph[node])
        return

    def get_weight(self, u, v):
        for node, weight in self.graph[u]:
            if node == v:
                return weight
        return None

    def get_neighbors(self, u):
        return self.graph[u]


graph = graph(True)
graph.add_edge(1, 2, 1)
graph.add_edge(1, 3, 1)
graph.add_edge(2, 3, 1)
graph.add_edge(2, 4, 1)
graph.add_edge(3, 4, 1)
