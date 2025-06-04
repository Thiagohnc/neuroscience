#include "params.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>

using namespace std;

string params_path;
map<string,string> param_values;
std::random_device rd;

void set_params_path(const string &_params_path) {
    params_path = _params_path;
}

void read_params() {
    string line;
    ifstream file(params_path);
    if(file.is_open()) {
        while(getline(file,line)) {
			line = strip(line);
			if(line.size() == 0 || line[0] == '#') continue;
            vstring p_val = split(line, '=');
            param_values[p_val[0]] = p_val[1];
        }
    }
}

string get_param(const string &param_name) {
    if(param_values.find(param_name) == param_values.end()) {
        read_params();
        if(param_values.find(param_name) == param_values.end()) {
            throw invalid_argument("Param " + param_name + " not found in " + params_path);
        }
    }
    return param_values[param_name];
}

string param_graph() {
    return get_param("graph");
}

vvdouble param_p() {
    string str_param = get_param("p");
    string format_msg = "parameter 'p' must be a square matrix in the format: [row1 ; row2], "
                        "where rows use semicolons and values use commas. Example: [0.5, 0.1 ; 0.1, 0.5]";
    if(str_param.front() != '[' or str_param.back() != ']')
        throw invalid_argument("Notation lacks brackets: " + format_msg);
    vvdouble p = str_to_matrix<double>(str_param.substr(1, str_param.size() - 2), ',', ';');

    for(int i = 0; i < (int)p.size(); i++) {
        if(p[i].size() != p.size())
            throw invalid_argument("Matriz is not square: " + format_msg);
    }

    vint group_n = param_group_n();
    if(p.size() != group_n.size()) {
        throw invalid_argument("Parameter 'p' dimension (" + to_string(p.size()) + ") is different "
                               "than the number of groups in 'group_n' (" + to_string(group_n.size()) + ")");
    }

    return p;
}

double param_mu_in() {
    return stod(get_param("mu_in"));
}

double param_mu_out() {
    return stod(get_param("mu_out"));
}

double param_lambda() {
    return stod(get_param("lambda"));
}

vint param_group_n() {
    string str_param = get_param("group_n");
    if(str_param.front() != '[' or str_param.back() != ']')
        throw invalid_argument("Invalid format for 'group_n'. Expected a list of integers, e.g. "
                               "[150, 150] for two groups of 150 nodes.");
    return str_to_vec<int>(str_param.substr(1, str_param.size() - 2), ',');
}

int param_T() {
    return stoi(get_param("T"));
}

int param_BURN_T() {
    return stoi(get_param("BURN_T"));
}

int param_delay() {
    return stoi(get_param("delay"));
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

int param_should_be_strongly_connected() {
    return stod(get_param("should_be_strongly_connected"));
}

int param_spike_trains_file() {
	return (int) stoi(get_param("spike_trains_file"));
}

int param_firing_rate_file() {
	return (int) stoi(get_param("firing_rate_file"));
}

int param_adjacency_0_1_file() {
	return (int) stoi(get_param("adjacency_0_1_file"));
}

int param_adjacency_weights_file() {
	return (int) stoi(get_param("adjacency_weights_file"));
}

int param_pearson_file() {
	return (int) stoi(get_param("pearson_file"));
}

int param_spike_average_file() {
	return (int) stoi(get_param("spike_average_file"));
}

int param_spike_variance_file() {
	return (int) stoi(get_param("spike_variance_file"));
}

string param_output_folder() {
    return get_param("output_folder");
}

int param_silent() {
    return stoi(get_param("silent"));
}

int param_zip_results() {
    return stoi(get_param("zip_results"));
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

