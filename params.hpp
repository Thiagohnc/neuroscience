#ifndef PARAMS_HPP
#define PARAMS_HPP

#include "types.hpp"
#include <string>

void set_params_path(const std::string &_params_path);
void read_params();
std::string get_param(const std::string &param_name);
std::string param_graph();
vvdouble param_p();
vvdouble param_mu();;
double param_lambda();
vint param_group_n();
int param_T();
int param_BURN_T();
int param_delay();
double param_w_intra_mean();
double param_w_intra_std();
double param_w_inter_mean();
double param_w_inter_std();
double param_auto_activ();
double param_intra_exchitatory_portion();
double param_inter_exchitatory_portion();
int param_should_be_strongly_connected();
int param_spike_trains_file();
int param_firing_rate_file();
int param_adjacency_0_1_file();
int param_adjacency_weights_file();
int param_pearson_file();
int param_spike_average_file();
int param_spike_variance_file();
std::string param_output_folder();
int param_silent();
int param_zip_results();
unsigned int param_samples();
long unsigned int param_seed();

#endif