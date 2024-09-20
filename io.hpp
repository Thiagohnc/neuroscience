#ifndef IO_HPP
#define IO_HPP

#include "graph.hpp"
#include <string>
#include <vector>

using namespace std;

void output_spike_trains(const vector<vector<bool>> &spike_trains, const string &output_folder);
void output_spike_average(const vector<vector<bool>> &spike_trains, const string &output_folder);
void output_spike_variance(const vector<vector<bool>> &spike_trains, const string &output_folder);
void output_adjacency_0_1(Graph &g, const string &output_folder);
void output_adjacency_weights(Graph &g, const string &output_folder);

#endif