#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>

void set_params_path(std::string _params_path);
void read_params();
std::string get_param(std::string param_name);
double param_p();
double param_q();
int param_N();
int param_T();
double param_alpha_ewma();
double param_w_intra_mean();
double param_w_intra_std();
double param_w_inter_mean();
double param_w_inter_std();
double param_auto_activ();
double param_intra_exchitatory_portion();
double param_inter_exchitatory_portion();
std::string param_output_folder();
unsigned int param_seed();

#endif