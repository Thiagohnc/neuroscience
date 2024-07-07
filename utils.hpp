#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;

int rand(int cap);
std::vector<int> rand_vector(int n, int cap);
double rand01();
bool coin_flip(double p);
int mod(int x, int m);
double mean(std::vector<int> &v);
std::vector<int> vector_to_ints(std::vector<std::string> v);
std::vector<std::vector<int>> matrix_to_ints(std::vector<std::vector<std::string>> v);
std::vector<std::string> split(std::string str, char delim);
std::vector<std::vector<std::string>> string_to_matrix(std::string str);
double logistic(double x);
double inverse_logistic(double y);
void progress_bar(int done, int total, std::string name);
void mkdir_tree(std::string folder);
void set_seed(int seed);

#endif