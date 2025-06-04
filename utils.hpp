#ifndef UTILS_HPP
#define UTILS_HPP

#define PRINT(str) if(!param_silent()) cout << str;
#define PRINTLN(str) if(!param_silent()) cout << str << endl;

#include "types.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;

long unsigned int rand(int cap);
vlonguint rand_vector(int n, unsigned long int cap);
double rand01();
bool coin_flip(double p);
int mod(int x, int m);
double mean(const vbool &v);
vstring split(std::string str, char delim);
double logistic(double x);
double inverse_logistic(double y);
void progress_bar(int done, int total, const std::string &name);
std::string strip(std::string);
void mkdir_tree(std::string folder);
void zip_and_remove(std::string path);
void exec_shell(std::string cmd);
void set_seed(long unsigned int seed);

template<typename T>
T convert_from_string(const std::string& str) {
    std::istringstream iss(str);
    T val;
    iss >> val;
    return val;
}

template<>
inline bool convert_from_string<bool>(const std::string& str) {
    return static_cast<bool>(stoi(str));
}

template<typename T> std::vector<T> str_to_vec(const std::string &str, char delim) {
    vstring str_vec = split(str, delim);
    std::vector<T> ans(str_vec.size());

    for(int i = 0; i < (int)str_vec.size(); i++) {
        ans[i] = convert_from_string<T>(str_vec[i]);
    }

    return ans;
}

template<typename T> std::vector<std::vector<T>> str_to_matrix(const std::string &str, char delim_row, char delim_col) {
    vstring rows = split(str, delim_col);
    std::vector<std::vector<T>> ans(rows.size());

    for(int i = 0; i < (int)rows.size(); i++) {
        vstring str_values = split(rows[i], delim_row);
        ans[i].resize(str_values.size());
        for(int j = 0; j < (int)str_values.size(); j++) {
            ans[i][j] = convert_from_string<T>(str_values[j]);
        }
    }

    return ans;
}

#endif