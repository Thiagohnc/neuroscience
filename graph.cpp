#include "graph.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

#include <iostream>

/* Edge class */

Edge::Edge(int d, double w) {
    destiny = d;
    weight = w;
}

Edge::Edge() {}

bool Edge::operator < (const Edge &b) const {
    return destiny < b.destiny;
}

int Edge::d() {
    return destiny;
}

double Edge::w() {
    return weight;
}


/* Graph class */

Graph::Graph(int n) {
    neighbors.resize(n);
}

Graph::Graph() {}

void Graph::add_edge(int o, int d, double w) {
    neighbors[o].push_back(Edge(d, w));
}

int Graph::neighbor_quantity(int v) {
    return (int) neighbors[v].size();
}

int Graph::kth_neighbor(int v, int k) {
    assert(k <= neighbor_quantity(v));
    return neighbors[v][k].d();
}

double Graph::kth_weight(int v, int k) {
    assert(k <= neighbor_quantity(v));
    return neighbors[v][k].w();
}