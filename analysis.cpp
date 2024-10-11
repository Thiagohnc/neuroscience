#include "analysis.hpp"
#include "params.hpp"
#include "utils.hpp"
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

double pearson(int a, int b, vector<vector<bool>> &spikes, vector<double> &means, vector<double> &dens, vector<double> &delayed_dens) {
	const int delay = param_delay();
	
	double num = 0;
	
	for(int t = 0; t < (int)spikes[a].size() - delay; t++) {
		num += (spikes[a][t] - means[a]) * (spikes[b][t + delay] - means[b]);
	}
	
	return num / sqrt(dens[a] * delayed_dens[b]);
}

vector<vector<double>> pearson_all_pairs(vector<vector<bool>> &spikes) {
	const int delay = param_delay();
	
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
		progress_bar(i + 1, (int)spikes.size(), "Output: Pearson - Pré-processamento");
	}
	
	for(int i = 0; i < (int)spikes.size(); i++) {
		for(int j = 0; j < (int)spikes.size(); j++) {
			corr[i].push_back(pearson(i, j, spikes, means, dens, delayed_dens));
		}
		progress_bar(i + 1, (int)spikes.size(), "Output: Pearson - Cálculo");
	}
	
	return corr;
}

vector<vector<bool>> read_spike_trains_file(string file_path) {
	vector<vector<bool>> spikes;
	int readed = 0;
    ifstream file(file_path);
	string line;
	
	progress_bar(0, 1, "Leitura do input (spike trains)");
    if(file.is_open()) {
        while(getline(file,line)) {
			spikes.push_back(vector_to_bools(split(line, ' ')));
			readed += line.size() + 1;
        }
    }
	progress_bar(1, 1, "Leitura do input (spike trains)");
	return spikes;
}

void write_pearson_correlation(vector<vector<bool>> &spikes, string file_path) {
	ofstream pearson_file(file_path);
	
	vector<vector<double>> corr = pearson_all_pairs(spikes);
	for(int i = 0; i < (int)corr.size(); i++) {
		for(int j = 0; j < (int)corr.size(); j++) {
			pearson_file << corr[i][j] << " ";
		}
		pearson_file << endl;
		progress_bar(i + 1, (int)spikes.size(), "Escrita no output (pearson)");
	}
}