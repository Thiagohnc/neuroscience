#include "params.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace std;
using namespace std::chrono;

mt19937 twister;
default_random_engine generator;
uniform_real_distribution<double> rand_real(0,1);

long unsigned int rand(int cap) {
	return twister() % cap;
}

vector<long unsigned int> rand_vector(int n, unsigned long int cap) {
    vector<long unsigned int> v(n);
    for(int i = 0; i < n; i++)
        v[i] = rand(cap);
    return v;
}

double rand01() {
    return rand_real(generator);
    //return (double) (twister() % twister.max()) / twister.max();
}

bool coin_flip(double p) {
    return rand01() < p;
}

int mod(int x, int m) {
    while(x < 0)
        x += m;
    return x % m;
}

double mean(vector<int> &v) {
	long long int sum = 0;
	for(int i = 0; i < (int)v.size(); i++) {
		sum += v[i];
	}
	return (double)sum/v.size();
}

vector<int> vector_to_ints(vector<string> v) {
    vector<int> ans;
    for(int i = 0; i < (int)v.size(); i++) {
        ans.push_back(stoi(v[i]));
    }
    return ans;
}

vector<vector<int>> matrix_to_ints(vector<vector<string>> v) {
    vector<vector<int>> ans;
    for(int i = 0; i < (int)v.size(); i++) {
        ans.push_back(vector_to_ints(v[i]));
    }
    return ans;
}

vector<string> split(string str, char delim) {
    vector<string> ans;
    int next_char_idx = 0;
    for(int i = 0; i < (int)str.size(); i++) {
        if(str[i] == delim) {
            ans.push_back(str.substr(next_char_idx, i - next_char_idx));
            next_char_idx = i + 1;
        }
    }
    
    if(next_char_idx < (int)str.size()) {
        ans.push_back(str.substr(next_char_idx, str.size() - next_char_idx + 1));
    }
    
    return ans;
}

vector<vector<string>> string_to_matrix(string str) {
    vector<vector<string>> ans;
    int idx = 1; // Ignore first bracket
    while(idx < (int)str.size() - 1) { // Ignore last bracket
        if(str[idx] == '[') {
            size_t closing_bracket = str.find("]", idx + 1);
            auto line = split(str.substr(idx + 1, closing_bracket - idx - 1), ',');
            ans.push_back(line);
            idx = closing_bracket + 1;
        }
        else idx++;
    }
    return ans;
}

double logistic(double x) {
    return exp(x) / (1 + exp(x));
}

double inverse_logistic(double y) {
    assert(y > 0 && y < 1);
    return log(y / (1 - y));
}

void progress_bar(int done, int total, string name) {
	static timepoint start;
	static bool started = false;
	const int bar_size = 50;
	double progress = (double)done/total;
	int filled = bar_size * progress;
	int blank = bar_size - filled;
	
	if(!started) {
		started = true;
		start = high_resolution_clock::now();
	}
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	
	cout << "[";
	for(int i = 0; i < filled; i++) cout << "\u25A0";
	for(int i = 0; i < blank; i++) cout << " ";
	cout << "] " << (int)(100 * progress) << "% - " << name;
	cout << setprecision(3) << " (" << duration.count()/1e6 << "s)    \r";
	cout.flush();
	
	
	if(done == total) {
		cout << endl;
		started = false;
	}
}

string strip(string s) {
	int left = 0;
	for(int i = 0; i < (int)s.size(); i++) {
		if(isspace(s[i])) left = i + 1;
		else break;
	}
	s.erase(s.begin(), s.begin() + left);
	
	int right = s.size();
	for(int i = s.size() - 1; i >= 0; i--) {
		if(isspace(s[i])) right = i;
		else break;
	}
	s.erase(s.begin() + right, s.end());
	
	return s;
}

void mkdir_tree(string folder) {
    for(int i = 0; i <= (int) folder.size(); i++)
        if(folder[i] == '/')
            mkdir(folder.substr(0, i).data(), 0777);
    mkdir(folder.data(), 0777);
}

void set_seed(long unsigned int seed) {
    twister = mt19937{seed};
    generator = default_random_engine(seed);
}