#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

class Node {
private:
    double auto_activation_rate;
    double exit_weight;
public:
    Node(double b);
    Node();
    double b();
    double W();
    void set_b(double b);
    void add_weight(double b);
};

class Edge {
private:
    int destiny;
    double weight;
public:
    Edge(int d, double w);
    Edge();
    bool operator < (const Edge &other) const;
    int d();
    double w();
};

class Graph {
private:
    std::vector<std::vector<Edge> > neighbors;
    std::vector<Node> nodes;
public:
    Graph(int n);
    Graph();
    void add_edge(int o, int d, double w);
    int neighbor_quantity(int v);
    int kth_neighbor(int v, int k);
    double kth_weight(int v, int k);
    Node& kth_node(int k);
};

#endif