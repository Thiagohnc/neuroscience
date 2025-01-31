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

	double at(int r, int c) const {
		return m[r][c];
	}

	Matrix operator+ (const Matrix& B) const {
		Matrix C(n);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				C.set(i, j, at(i, j) + B.at(i, j));
			}
		}
		return C;
	}

	Matrix& operator+= (const Matrix& B) {
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				set(i, j, at(i, j) + B.at(i, j));
			}
		}
		return *this;
	}

	Matrix operator- (const Matrix& B) const {
		Matrix C(n);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				C.set(i, j, at(i, j) - B.at(i, j));
			}
		}
		return C;
	}

	Matrix& operator-= (const Matrix& B) {
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				set(i, j, at(i, j) - B.at(i, j));
			}
		}
		return *this;
	}

	Matrix operator* (const Matrix &B) const {
		/*Matrix C(n, 0);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < n; j++) {
				for(int k = 0; k < n; k++) {
					C[i][j] += m[i][k] * B[k][j];
				}
			}
		}
		return C;*/

		Matrix C(n);
		if(n <= 64) {
			for(int i = 0; i < n; i++) {
				for(int j = 0; j < n; j++) {
					for(int k = 0; k < n; k++) {
						C.set(i, j, C.at(i, j) + at(i, k) * B.at(k, j));
					}
				}
			}
		}
		else {
			int half = n/2;
			Matrix A11 = copy_to(0, 0, half, half);
			Matrix A12 = copy_to(0, half, half, n);
			Matrix A21 = copy_to(half, 0, n, half);
			Matrix A22 = copy_to(half, half, n, n);

			Matrix B11 = B.copy_to(0, 0, half, half);
			Matrix B12 = B.copy_to(0, half, half, n);
			Matrix B21 = B.copy_to(half, 0, n, half);
			Matrix B22 = B.copy_to(half, half, n, n);

			Matrix P1 = (A11 + A22) * (B11 + B22);
			Matrix P2 = (A21 + A22) * B11;
			Matrix P3 = A11 * (B12 - B22);
			Matrix P4 = A22 * (B21 - B11);
			Matrix P5 = (A11 + A12) * B22;
			Matrix P6 = (A21 - A11) * (B11 + B12);
			Matrix P7 = (A12 - A22) * (B21 + B22);

			C.copy_from(P1 + P4 - P5 + P7, 0, 0);
			C.copy_from(P3 + P5, 0, half);
			C.copy_from(P2 + P4, half, 0);
			C.copy_from(P1 - P2 + P3 + P6, half, half);
		}

		return C;
	}

	void set(int i, int j, double val) {
		m[i][j] = val;
	}

	Matrix copy_to(int x0, int y0, int x1, int y1) const {
		Matrix B(x1 - x0);
		for(int i = 0; i < x1 - x0; i++) {
			for(int j = 0; j < y1 - y0; j++) {
				B.set(i, j, at(x0 + i, y0 + j));
			}
		}
		return B;
	}

	void copy_from(const Matrix& A, int x0, int y0) {
		for(int i = 0; i < A.n; i++) {
			for(int j = 0; j < A.n; j++) {
				set(x0 + i, y0 + j, A.at(i, j));
			}
		}
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
	double op1 = pow(blocksize, log2(7));
	double op2 = pow(blocksize/2, log2(7)) + N * (N - blocksize);
	bool fully_calculated_with_strassen = true;
	if(op2 < op1) {
		fully_calculated_with_strassen = false;
		blocksize /= 2;
	}

	int blocks = (T - delay + blocksize - 1)/blocksize;
	Matrix M(blocksize, 0);
	for(int b = 0; b < blocks; b++) {
		Matrix A(blocksize), B(blocksize);
		for(int i = 0, ispk = 0; i < blocksize; i++, ispk++) {
			for(int j = 0, jspk = b * blocksize; j < blocksize; j++, jspk++) {
				A.set(i, j, (ispk < N && jspk < T) ? spikes[ispk][jspk] - means[ispk] : 0);
				B.set(j, i, (ispk < N && jspk + delay < T) ? spikes[ispk][jspk + delay] - means[ispk] : 0);
			}
		}

		M += (A * B);

		if(fully_calculated_with_strassen) progress_bar(b + 1, blocks, "Output: Pearson - Cálculo");
		else progress_bar(b + 1, blocks + N, "Output: Pearson - Cálculo");
	}

	if(!fully_calculated_with_strassen) {
		for(int i = 0; i < N; i++) {
			for(int j = 0; j < N; j++) {
				if(i >= blocksize || j >= blocksize) {
					corr[i][j] = pearson(i, j, spikes, means, dens, delayed_dens);
				}
			}
			progress_bar(blocks + i + 1, blocks + N, "Output: Pearson - Cálculo");
		}
	}

	const int N_Strassen = min(N, blocksize);

	for(int i = 0; i < N_Strassen; i++) {
		for(int j = 0; j < N_Strassen; j++) {
			corr[i][j] = M.at(i, j) / sqrt(dens[i] * delayed_dens[j]);
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