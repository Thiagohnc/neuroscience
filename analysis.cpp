#include "analysis.hpp"
#include "params.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <Eigen/Dense>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

vvdouble pearson_all_pairs(vvbool &spikes) {
	const int delay = param_delay();
	
	progress_bar(0, 4, "Output: Pearson - Alocação de variáveis");
	vdouble means(spikes.size());
	progress_bar(1, 4, "Output: Pearson - Alocação de variáveis");
	vdouble dens(spikes.size());
	progress_bar(2, 4, "Output: Pearson - Alocação de variáveis");
	vdouble delayed_dens(spikes.size());
	progress_bar(3, 4, "Output: Pearson - Alocação de variáveis");
	vvdouble corr(spikes.size(), vdouble(spikes.size()));
	progress_bar(4, 4, "Output: Pearson - Alocação de variáveis");
	
	int T = spikes[0].size();
	int N = spikes.size();

	progress_bar(0, N, "Output: Pearson - Pré-processamento");
	for(int i = 0; i < N; i++) {
		means[i] = mean(spikes[i]);
		dens[i] = delayed_dens[i] = 0;
		for(int t = 0; t < (int)spikes[i].size() - delay; t++) {
			dens[i] += (spikes[i][t] - means[i]) * (spikes[i][t] - means[i]);
			delayed_dens[i] += (spikes[i][t + delay] - means[i]) * (spikes[i][t + delay] - means[i]);
		}
		progress_bar(i + 1, N, "Output: Pearson - Pré-processamento");
	}

	int blocksize = min((size_t) T,  T/(20 * sizeof(double))); // Each matrix will occupy 5% the size of spike_trains matrix of bool
	Eigen::MatrixXd A(N, blocksize), B(blocksize, N);
	int blocks = blocksize == 0 ? 0 : (T - delay + blocksize - 1)/blocksize;
	Eigen::MatrixXd M = Eigen::MatrixXd::Zero(N, N);
	progress_bar(0, blocks, "Output: Pearson - Cálculo");
	for(int b = 0; b < blocks; b++) {
		if(blocksize * blocks != T && b == blocks - 1) {
			int new_blocksize = T % blocksize;
			A.resize(N, new_blocksize);
			B.resize(new_blocksize, N);
			for(int i = 0, ispk = 0; i < N; i++, ispk++) {
				for(int j = 0, jspk = b * blocksize; j < new_blocksize; j++, jspk++) {
					A(i,j) = (ispk < N && jspk < T) ? spikes[ispk][jspk] - means[ispk] : 0;
					B(j,i) = (ispk < N && jspk + delay < T) ? spikes[ispk][jspk + delay] - means[ispk] : 0;
				}
			}
		}
		else {
			for(int i = 0, ispk = 0; i < N; i++, ispk++) {
				for(int j = 0, jspk = b * blocksize; j < blocksize; j++, jspk++) {
					A(i,j) = spikes[ispk][jspk] - means[ispk];
					B(j,i) = spikes[ispk][jspk + delay] - means[ispk];
				}
			}
		}

		M += (A * B);

		progress_bar(b + 1, blocks, "Output: Pearson - Cálculo");
 	}

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			corr[i][j] = M(i, j) / sqrt(dens[i] * delayed_dens[j]);
		}
		progress_bar(i + 1, N, "Output: Pearson - Finalização");
	}

	return corr;
}

vvbool read_spike_trains_file(string file_path) {
	vvbool spikes;
    ifstream file(file_path);
	string line;
	
	progress_bar(0, 1, "Leitura do input (spike trains)");
    if(file.is_open()) {
        while(getline(file,line)) {
			spikes.push_back(vector_to_bools(split(line, ' ')));
        }
    }
	progress_bar(1, 1, "Leitura do input (spike trains)");
	return spikes;
}

void write_pearson_correlation(vvbool &spikes, string file_path) {
	ofstream pearson_file(file_path);
	
	vvdouble corr = pearson_all_pairs(spikes);
	for(int i = 0; i < (int)corr.size(); i++) {
		for(int j = 0; j < (int)corr.size(); j++) {
			pearson_file << corr[i][j] << " ";
		}
		pearson_file << endl;
		progress_bar(i + 1, (int)spikes.size(), "Escrita no output (pearson)");
	}
}