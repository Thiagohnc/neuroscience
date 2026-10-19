#include "analysis.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <Eigen/Dense>
#include <cmath>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;

vvdouble similarity_all_pairs(vvbool &spikes, int delay, const string &similarity_matrix_type) {
    const string output_name = (similarity_matrix_type == "correlation" ? "correlation_" : "covariance_") + to_string(delay);
	progress_bar(0, 4, "Output: " + output_name + " - Alocação de variáveis");
	vdouble means(spikes.size());
	progress_bar(1, 4, "Output: " + output_name + " - Alocação de variáveis");
	vdouble dens(spikes.size());
	progress_bar(2, 4, "Output: " + output_name + " - Alocação de variáveis");
	vdouble delayed_dens(spikes.size());
	progress_bar(3, 4, "Output: " + output_name + " - Alocação de variáveis");
	vvdouble corr(spikes.size(), vdouble(spikes.size()));
	progress_bar(4, 4, "Output: " + output_name + " - Alocação de variáveis");
	
	int T = spikes[0].size();
	int N = spikes.size();

	progress_bar(0, N, "Output: " + output_name + " - Pré-processamento");
	for(int i = 0; i < N; i++) {
		means[i] = mean(spikes[i]);
		dens[i] = delayed_dens[i] = 0;
		for(int t = 0; t < (int)spikes[i].size() - delay; t++) {
			dens[i] += (spikes[i][t] - means[i]) * (spikes[i][t] - means[i]);
			delayed_dens[i] += (spikes[i][t + delay] - means[i]) * (spikes[i][t + delay] - means[i]);
		}
		progress_bar(i + 1, N, "Output: " + output_name + " - Pré-processamento");
	}

    int blocksize = max(1, T/(20 * (int)sizeof(double))); // Each matrix will occupy about 5% the size of spike_trains matrix of bool
    int overlap = T - delay;
    int blocks = (overlap + blocksize - 1)/blocksize;
    Eigen::MatrixXd M = Eigen::MatrixXd::Zero(N, N);
    progress_bar(0, blocks, "Output: " + output_name + " - Cálculo");
    for(int b = 0; b < blocks; b++) {
        int current_blocksize = min(blocksize, overlap - b * blocksize);
        Eigen::MatrixXd A(N, current_blocksize), B(current_blocksize, N);
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < current_blocksize; j++) {
                int t = b * blocksize + j;
                A(i,j) = spikes[i][t] - means[i];
                B(j,i) = spikes[i][t + delay] - means[i];
            }
        }
        M += (A * B);
        progress_bar(b + 1, blocks, "Output: " + output_name + " - Cálculo");
    }

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			// Both options use full-train means; covariance is normalized by the overlap.
            corr[i][j] = M(i, j) / (similarity_matrix_type == "covariance" ? overlap : sqrt(dens[i] * delayed_dens[j]));
		}
		progress_bar(i + 1, N, "Output: " + output_name + " - Finalização");
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
			spikes.push_back(str_to_vec<bool>(line, ' '));
        }
    }
	progress_bar(1, 1, "Leitura do input (spike trains)");
	return spikes;
}

void write_similarity_matrix(vvbool &spikes, string file_path, int delay, const string &similarity_matrix_type) {
    const string output_name = (similarity_matrix_type == "correlation" ? "correlation_" : "covariance_") + to_string(delay);
	ofstream similarity_matrix_file(file_path);
	
	vvdouble corr = similarity_all_pairs(spikes, delay, similarity_matrix_type);
	for(int i = 0; i < (int)corr.size(); i++) {
		for(int j = 0; j < (int)corr.size(); j++) {
			similarity_matrix_file << corr[i][j] << " ";
		}
		similarity_matrix_file << endl;
		progress_bar(i + 1, (int)spikes.size(), "Escrita no output (" + output_name + ")");
	}
}