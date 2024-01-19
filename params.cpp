#include "params.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

using namespace std;

map<string,string> param_values;

void read_params() {
    string line;
    ifstream file("params.txt");
    if(file.is_open()) {
        while(getline(file,line)) {
            pair<string,string> p_val = split(line, '=');
            param_values[p_val.first] = p_val.second;
        }
    }
}

string get_param(string param_name) {
    if(param_values.find(param_name) == param_values.end()) {
        read_params();
        if(param_values.find(param_name) == param_values.end()) {
            throw invalid_argument("Param " + param_name + " not found in params.txt");
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