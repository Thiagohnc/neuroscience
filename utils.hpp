#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;

long unsigned int rand(int cap);
std::vector<long unsigned int> rand_vector(int n, unsigned long int cap);
double rand01();
bool coin_flip(double p);
int mod(int x, int m);
double mean(std::vector<short int> &v);
std::vector<int> vector_to_ints(std::vector<std::string> v);
std::vector<short int> vector_to_short_ints(std::vector<std::string> v);
std::vector<std::vector<int>> matrix_to_ints(std::vector<std::vector<std::string>> v);
std::vector<std::string> split(std::string str, char delim);
std::vector<std::vector<std::string>> string_to_matrix(std::string str);
double logistic(double x);
double inverse_logistic(double y);
void progress_bar(int done, int total, std::string name);
std::string strip(std::string);
void mkdir_tree(std::string folder);
void zip_and_remove(std::string path);
void set_seed(unsigned long int seed);

#endif