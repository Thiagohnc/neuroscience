#include "graph.hpp"
#include "params.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

#include <iostream>

using namespace std;

/* Edge class */

Edge::Edge(int d, double w) {
    destiny = d;
    weight = w;
}

Edge::Edge() {}

bool Edge::operator < (const Edge &b) const {
    return destiny < b.destiny;
}

int Edge::d() const {
    return destiny;
}

double Edge::w() const {
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

int Graph::neighbor_quantity(int v) const {
    return (int) neighbors[v].size();
}

int Graph::kth_neighbor(int v, int k) const {
    assert(k <= neighbor_quantity(v));
    return neighbors[v][k].d();
}

double Graph::kth_weight(int v, int k) const {
    assert(k <= neighbor_quantity(v));
    return neighbors[v][k].w();
}

Graph Graph::reversed_graph() {
    const int N = param_N();
    Graph rev(N);
    for(int u = 0; u < N; u++) {
        for(int k = 0; k < neighbor_quantity(u); k++) {
            int v = kth_neighbor(u, k);
            double w = kth_weight(u, k);
            rev.add_edge(v, u, w);
        }
    }
    return rev;
}

/* Algorithms */

void dfs(const Graph &g, int u, vector<bool> &mrk) {
    mrk[u] = true;
    for(int k = 0; k < g.neighbor_quantity(u); k++) {
        int v = g.kth_neighbor(u, k);
        if(mrk[v] == false)
            dfs(g, v, mrk);
    }
}

bool all_true(vector<bool> &v) {
    return all_of(v.begin(), v.end(), [] (bool val) {return val;});
}

bool strongly_connected(Graph &g) {
    const int N = param_N();

    vector<bool> mrk(N, false);
    vector<bool> rev_mrk(N, false);
    dfs(g, 0, mrk);
    dfs(g.reversed_graph(), 0, rev_mrk);

    return all_true(mrk) and all_true(rev_mrk);
}