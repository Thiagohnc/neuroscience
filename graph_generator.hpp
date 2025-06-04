#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "graph.hpp"
#include "types.hpp"
#include <string>
#include <vector>

Graph stochastic_block_model(vvint &groups);
Graph sc_stochastic_block_model(vvint &groups, int max_tries = 100);
Graph graph_from_file(std::string filepath);

#endif