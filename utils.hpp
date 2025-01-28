#ifndef UTILS_HPP
#define UTILS_HPP

#define PRINT(str) if(!param_silent()) cout << str;
#define PRINTLN(str) if(!param_silent()) cout << str << endl;

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

enum Weight {IntraExc, InterExc, IntraInb, InterInb};

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;

long unsigned int rand(int cap);
std::vector<long unsigned int> rand_vector(int n, unsigned long int cap);
double rand01();
bool coin_flip(double p);
int mod(int x, int m);
double mean(const std::vector<bool> &v);
std::vector<int> vector_to_ints(std::vector<std::string> v);
std::vector<double> vector_to_doubles(std::vector<std::string> v);
std::vector<bool> vector_to_bools(std::vector<std::string> v);
std::vector<std::vector<int>> matrix_to_ints(std::vector<std::vector<std::string>> v);
std::vector<std::string> split(std::string str, char delim);
std::vector<std::vector<std::string>> string_to_matrix(std::string str);
double logistic(double x);
double inverse_logistic(double y);
void progress_bar(int done, int total, std::string name);
std::string strip(std::string);
void mkdir_tree(std::string folder);
void zip_and_remove(std::string path);
void exec_shell(std::string cmd);
void set_seed(long unsigned int seed);

#endif