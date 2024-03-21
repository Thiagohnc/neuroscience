#include "params.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <stdexcept>
#include <string>

using namespace std;

mt19937 twister;
default_random_engine generator;
uniform_real_distribution<double> rand_real(0,1);

int rand(int cap) {
	return twister() % cap;
}

double rand01() {
    return rand_real(generator);
    //return (double) (twister() % twister.max()) / twister.max();
}

bool coin_flip(double p) {
    return rand01() < p;
}

pair<string,string> split(string str, char delim) {
    for(int i = 0; i < (int)str.size(); i++) {
        if(str[i] == delim) {
            string a = str.substr(0, i);
            string b = str.substr(i + 1, str.size() - i - 1);
            return make_pair(a, b);
        }
    }
    string delim_str(1, delim);
    throw invalid_argument("No character " + delim_str + " found in string " + str);
}

double logistic(double x) {
    return exp(x) / (1 + exp(x));
}

double inverse_logistic(double y) {
    assert(y > 0 && y < 1);
    return log(y / (1 - y));
}

void set_seed(int seed) {
    twister = mt19937{seed};
    generator = default_random_engine(seed);
}