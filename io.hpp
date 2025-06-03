#ifndef IO_HPP
#define IO_HPP

#include "graph.hpp"
#include "types.hpp"
#include <string>
#include <vector>

using namespace std;

void output_spike_trains(const vvbool &spike_trains, const string &output_folder);
void output_firing_rate(const Graph &g, const vvbool &spike_trains, const string &output_folder);
void output_spike_average(const vvbool &spike_trains, const string &output_folder);
void output_spike_variance(const vvbool &spike_trains, const string &output_folder);
void output_adjacency_0_1(const Graph &g, const string &output_folder);
void output_adjacency_weights(const Graph &g, const string &output_folder);

#endif