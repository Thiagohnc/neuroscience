#include "graph.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

#include <iostream>

/* Node class */

Node::Node(double b) {
    auto_activation_rate = b;
}

Node::Node() {}

double Node::b() {
    return auto_activation_rate;
}

void Node::set_b(double b) {
    auto_activation_rate = b;
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
    //std::cout << "não deveria bugar: " << o << " " << neighbors.size() << std::endl;
    neighbors[o].push_back(Edge(d, w));
    //std::cout << "não bugou :)" << std::endl;
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