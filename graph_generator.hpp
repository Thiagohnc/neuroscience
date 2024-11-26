#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "graph.hpp"
#include <string>
#include <vector>

Graph stochastic_block_model(std::vector<std::vector<int> > &groups, std::vector<std::vector<double> > &p);
Graph sc_stochastic_block_model(std::vector<std::vector<int> > &groups, std::vector<std::vector<double> > &p, int max_tries = 100);
Graph graph_from_file(std::string filepath);

#endif