#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

class Edge {
private:
    int destiny;
    double weight;
public:
    Edge(int d, double w);
    Edge();
    bool operator < (const Edge &other) const;
    int d() const;
    double w() const;
};

class Graph {
private:
    std::vector<std::vector<Edge> > neighbors;
    int node_quantity;
public:
    Graph(int n);
    Graph();
    void add_edge(int o, int d, double w);
    int N() const;
    int neighbor_quantity(int v) const;
    int kth_neighbor(int v, int k) const;
    double kth_weight(int v, int k) const;
    Graph reversed_graph() const;
};

bool strongly_connected(const Graph &g);

#endif