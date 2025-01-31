#include "analysis.hpp"
#include "params.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

typedef pair<int,int> pi;
typedef pair<pi,pi> pii;

class Matrix {
public:
	vector<vector<double>> m;
	int n;

	Matrix() {}
	Matrix(int n) : n(n) { m.resize(n, vector<double>(n)); }
	Matrix(int n, int v) : n(n) { m.resize(n, vector<double>(n, v)); }
	~Matrix() {
		for(int i = 0; i < n; i++) m[i].clear();
		m.clear();
	}
	vector<double>& operator[] (int r) { return m[r]; }

	Matrix operator+ (Matrix& B) const {
		Matrix C(n);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				C[i][j] = m[i][j] + B[i][j];
			}
		}
		return C;
	}

	Matrix& operator+= (Matrix& B) {
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				m[i][j] += B[i][j];
			}
		}
		return *this;
	}

	Matrix operator- (Matrix& B) const {
		Matrix C(n);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				C[i][j] = m[i][j] - B[i][j];
			}
		}
		return C;
	}

	Matrix& operator-= (Matrix& B) {
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				m[i][j] -= B[i][j];
			}
		}
		return *this;
	}

	Matrix operator* (Matrix &B) const {
		Matrix C(n, 0);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				for(int k = 0; k < n; k++) {
					C[i][j] += m[i][k] + B[k][j];
				}
			}
		}
		return C;
	}
};

double pearson(int a, int b, vector<vector<bool>> &spikes, vector<double> &means, vector<double> &dens, vector<double> &delayed_dens) {
	const static int delay = param_delay();
	
	double num = 0;
	
	for(int t = 0; t < (int)spikes[a].size() - delay; t++) {
		num += (spikes[a][t] - means[a]) * (spikes[b][t + delay] - means[b]);
	}
	
	return num / sqrt(dens[a] * delayed_dens[b]);
}

int next_power_of_two(int x) {
	int p = 1;
	while(p < x) p *= 2;
	return p;
}

vector<vector<double>> pearson_all_pairs(vector<vector<bool>> &spikes) {
	const static int delay = param_delay();
	
	vector<double> means(spikes.size());
	vector<double> dens(spikes.size());
	vector<double> delayed_dens(spikes.size());
	vector<vector<double>> corr(spikes.size(), vector<double>(spikes[0].size()));
	
	int T = spikes[0].size();
	int N = spikes.size();

	for(int i = 0; i < (int)spikes.size(); i++) {
		means[i] = mean(spikes[i]);
		dens[i] = delayed_dens[i] = 0;
		for(int t = 0; t < (int)spikes[i].size() - delay; t++) {
			dens[i] += (spikes[i][t] - means[i]) * (spikes[i][t] - means[i]);
			delayed_dens[i] += (spikes[i][t + delay] - means[i]) * (spikes[i][t + delay] - means[i]);
		}
		progress_bar(i + 1, (int)spikes.size(), "Output: Pearson - Pré-processamento");
	}

	int blocksize = next_power_of_two(N);
	int blocks = (T - delay + blocksize - 1)/blocksize;
	Matrix M(blocksize, 0);
	for(int b = 0; b < blocks; b++) {
		Matrix A(blocksize), B(blocksize);
		for(int i = 0, ispk = 0; i < blocksize; i++, ispk++) {
			for(int j = 0, jspk = b * blocksize; j < blocksize; j++, jspk++) {
				A[i][j] = (ispk < N && jspk < T) ? spikes[ispk][jspk] - means[ispk] : 0;
				B[i][j] = (ispk < N && jspk + delay < T) ? spikes[ispk][jspk + delay] - means[ispk] : 0;
			}
		}
		Matrix mult = A * B;
		M += mult;
		progress_bar(b + 1, blocks, "Output: Pearson - Cálculo");
	}

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			corr[i][j] = M[i][j] / sqrt(dens[i] * delayed_dens[j]);
		}
		progress_bar(i + 1, N, "Output: Pearson - Finalização");
	}

	return corr;

	/*
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
	}*/
	
	//return corr;
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