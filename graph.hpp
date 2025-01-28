#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include "utils.hpp"

class Edge {
private:
    int destiny;
    Weight weight;
public:
    Edge(int d, Weight w);
    Edge();
    bool operator < (const Edge &other) const;
    int d() const;
    Weight w() const;
};

class Graph {
private:
    std::vector<std::vector<Edge> > neighbors;
    int node_quantity;
public:
    Graph(int n);
    Graph();
    void add_edge(int o, int d, Weight w);
    int N() const;
    int neighbor_quantity(int v) const;
    int kth_neighbor(int v, int k) const;
    Weight kth_weight(int v, int k) const;
    Graph reversed_graph() const;
};

bool strongly_connected(const Graph &g);

#endif