#include "params.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

string params_path;
map<string,string> param_values;
std::random_device rd;

void set_params_path(string _params_path) {
    params_path = _params_path;
}

void read_params() {
    string line;
    ifstream file(params_path);
    if(file.is_open()) {
        while(getline(file,line)) {
			line = strip(line);
			if(line.size() == 0 || line[0] == '#') continue;
            vector<string> p_val = split(line, '=');
            param_values[p_val[0]] = p_val[1];
        }
    }
}

string get_param(string param_name) {
    if(param_values.find(param_name) == param_values.end()) {
        read_params();
        if(param_values.find(param_name) == param_values.end()) {
            throw invalid_argument("Param " + param_name + " not found in " + params_path);
        }
    }
    return param_values[param_name];
}

double param_p() {
    return stod(get_param("p"));
}

double param_q() {
    return stod(get_param("q"));
}

double param_mu() {
    return stod(get_param("mu"));
}

double param_mu_in() {
    return stod(get_param("mu_in"));
}

double param_mu_out() {
    return stod(get_param("mu_out"));
}

int param_N() {
    return stoi(get_param("N"));
}

int param_T() {
    return stoi(get_param("T"));
}

int param_BURN_T() {
    return stoi(get_param("BURN_T"));
}

double param_alpha_ewma() {
    return stod(get_param("alpha_ewma"));
}

double param_w_intra_mean() {
    return stod(get_param("w_intra_mean"));
}

double param_w_intra_std() {
    return stod(get_param("w_intra_std"));
}

double param_w_inter_mean() {
    return stod(get_param("w_inter_mean"));
}

double param_w_inter_std() {
    return stod(get_param("w_inter_std"));
}

double param_auto_activ() {
    return stod(get_param("auto_activ"));
}

double param_intra_exchitatory_portion() {
    return stod(get_param("intra_exchitatory_portion"));
}

double param_inter_exchitatory_portion() {
    return stod(get_param("inter_exchitatory_portion"));
}

double param_pw() {
    return stod(get_param("pw"));
}

bool param_spike_trains_file() {
	return (bool) stoi(get_param("spike_trains_file"));
}

bool param_firing_rate_file() {
	return (bool) stoi(get_param("firing_rate_file"));
}

bool param_adjacency_0_1_file() {
	return (bool) stoi(get_param("adjacency_0_1_file"));
}

bool param_adjacency_weights_file() {
	return (bool) stoi(get_param("adjacency_weights_file"));
}

bool param_pearson_file() {
	return (bool) stoi(get_param("pearson_file"));
}

string param_output_folder() {
    return get_param("output_folder");
}

int param_silent() {
    return stoi(get_param("silent"));
}

unsigned int param_samples() {
    return stoi(get_param("samples"));
}

long unsigned int param_seed() {
    static long unsigned int seed = rd();
    string seed_str = get_param("seed");
    if(seed_str != "auto") seed = stoul(get_param("seed"));
    return seed;
}

