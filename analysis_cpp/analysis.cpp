#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

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
	
	for(int i = 0 + delay; i < (int)a.size() - delay; i++) {
		num += (a[i] - mean_a) * (b[i + delay] - mean_b);
		den_a += (a[i] - mean_a) * (a[i] - mean_a);
		den_b += (b[i + delay] - mean_b) * (b[i + delay] - mean_b);
	}
	
	return num / sqrt(den_a * den_b);
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
	for(int i = 0; i < (int)spikes.size(); i++) {
		if(i%20 == 0) cout << "i = " << i << endl;
		for(int j = 0; j < (int)spikes.size(); j++) {
			pearson_file << pearson(spikes[i], spikes[j], 1) << " ";
		}
		pearson_file << endl;
	}
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	
	cout << duration.count()/1e6 << " segundos para calcular os coeficientes" << endl;
	
	return 0;
}
