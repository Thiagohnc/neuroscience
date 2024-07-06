#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <unistd.h>

#include <chrono>
using namespace std::chrono;

using namespace std;

// Copied from utils.cpp
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

// Copied from utils.cpp
vector<int> vector_to_ints(vector<string> v) {
    vector<int> ans;
    for(int i = 0; i < (int)v.size(); i++) {
        ans.push_back(stoi(v[i]));
    }
    return ans;
}

double mean(vector<int> &v) {
	long long int sum = 0;
	for(int i = 0; i < (int)v.size(); i++) {
		sum += v[i];
	}
	return (double)sum/v.size();
}

double pearson(vector<int> &a, vector<int> &b, int delay) {
	double mean_a = mean(a);
	double mean_b = mean(b);
	double num = 0, den_a = 0, den_b = 0;
	
	for(int t = 0; t < (int)a.size() - delay; t++) {
		num += (a[t] - mean_a) * (b[t + delay] - mean_b);
		den_a += (a[t] - mean_a) * (a[t] - mean_a);
		den_b += (b[t + delay] - mean_b) * (b[t + delay] - mean_b);
	}
	
	return num / sqrt(den_a * den_b);
}

double pearson(int a, int b, vector<vector<int>> &spikes, vector<double> &means, vector<double> &dens, vector<double> &delayed_dens, int delay) {
	double num = 0;
	
	for(int t = 0; t < (int)spikes[a].size() - delay; t++) {
		num += (spikes[a][t] - means[a]) * (spikes[b][t + delay] - means[b]);
	}
	
	return num / sqrt(dens[a] * delayed_dens[b]);
}

vector<vector<double>> pearson_all_pairs(vector<vector<int>> &spikes, int delay) {
	vector<double> means(spikes.size());
	vector<double> dens(spikes.size());
	vector<double> delayed_dens(spikes.size());
	vector<vector<double>> corr(spikes.size());
	
	for(int i = 0; i < (int)spikes.size(); i++) {
		means[i] = mean(spikes[i]);
		dens[i] = delayed_dens[i] = 0;
		for(int t = 0; t < (int)spikes[i].size() - delay; t++) {
			dens[i] += (spikes[i][t] - means[i]) * (spikes[i][t] - means[i]);
			delayed_dens[i] += (spikes[i][t + delay] - means[i]) * (spikes[i][t + delay] - means[i]);
		}
	}
	
	for(int i = 0; i < (int)spikes.size(); i++) {
		for(int j = 0; j < (int)spikes.size(); j++) {
			corr[i].push_back(pearson(i, j, spikes, means, dens, delayed_dens, delay));
		}
	}
	
	return corr;
}

int main(int argc, char *argv[]) {
	vector<vector<int>> spikes;
	string line;
	
	auto start0 = high_resolution_clock::now();
	
    ifstream file("../out/teste/1/spike_trains");
    if(file.is_open()) {
        while(getline(file,line)) {
			spikes.push_back(vector_to_ints(split(line, ' ')));
        }
    }
	
	auto stop0 = high_resolution_clock::now();
	auto duration0 = duration_cast<microseconds>(stop0 - start0);
	
	cout << duration0.count()/1e6 << " segundos para carregar na memória" << endl;
	
	auto start = high_resolution_clock::now();
	
	ofstream pearson_file("pearson");
	
	vector<vector<double>> corr = pearson_all_pairs(spikes, 1);
	for(int i = 0; i < (int)spikes.size(); i++) {
		for(int j = 0; j < (int)spikes.size(); j++) {
			pearson_file << corr[i][j] << " ";
		}
		pearson_file << endl;
	}
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	
	cout << duration.count()/1e6 << " segundos para calcular os coeficientes" << endl;
	
	return 0;
}
