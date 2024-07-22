#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>

void set_params_path(std::string _params_path);
void read_params();
std::string get_param(std::string param_name);
double param_p();
double param_q();
double param_mu();
double param_mu_in();
double param_mu_out();
int param_N();
int param_T();
int param_BURN_T();
double param_alpha_ewma();
double param_w_intra_mean();
double param_w_intra_std();
double param_w_inter_mean();
double param_w_inter_std();
double param_auto_activ();
double param_intra_exchitatory_portion();
double param_inter_exchitatory_portion();
double param_pw();
bool param_spike_trains_file();
bool param_firing_rate_file();
bool param_adjacency_0_1_file();
bool param_adjacency_weights_file();
bool param_pearson_file();
std::string param_output_folder();
unsigned int param_samples();
long unsigned int param_seed();

#endif