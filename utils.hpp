#ifndef UTILS_HPP
#define UTILS_HPP

#define PRINT(str) if(!param_silent()) cout << str;
#define PRINTLN(str) if(!param_silent()) cout << str << endl;

#include "types.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;

long unsigned int rand(int cap);
vlonguint rand_vector(int n, unsigned long int cap);
double rand01();
bool coin_flip(double p);
int mod(int x, int m);
double mean(const vbool &v);
vint vector_to_ints(vstring v);
vdouble vector_to_doubles(vstring v);
vbool vector_to_bools(vstring v);
vvint matrix_to_ints(vvstring v);
vstring split(std::string str, char delim);
vvstring string_to_matrix(std::string str);
double logistic(double x);
double inverse_logistic(double y);
void progress_bar(int done, int total, std::string name);
std::string strip(std::string);
void mkdir_tree(std::string folder);
void zip_and_remove(std::string path);
void exec_shell(std::string cmd);
void set_seed(long unsigned int seed);

#endif