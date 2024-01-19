#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <random>
#include <stdexcept>
#include <string>

using namespace std;

mt19937 twister{};

int rand(int cap) {
	return twister() % cap;
}

double rand01() {
    return (double) (twister() % twister.max()) / twister.max();
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