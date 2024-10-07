#include "graph.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

#include <iostream>

/* Node class */

Node::Node(double b) {
    auto_activation_rate = b;
    exit_weight = 0;
}

Node::Node() {}

double Node::W() {
    return exit_weight;
}

void Node::add_weight(double w) {
    exit_weight += w;
}

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
    nodes.resize(n);
}

Graph::Graph() {}

void Graph::add_edge(int o, int d, double w) {
    neighbors[o].push_back(Edge(d, w));
    kth_node(o).add_weight(w);
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

Node& Graph::kth_node(int k) {
    return nodes[k];
}