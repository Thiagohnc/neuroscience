#include "types.hpp"
#include <string>

using namespace std;

double pearson(int a, int b, vvbool &spikes, vdouble &means, vdouble &dens, vdouble &delayed_dens);
vvdouble similarity_all_pairs(vvbool &spikes, int delay, const string &similarity_matrix_type = "correlation");
vvbool read_spike_trains_file(string file_path);
void write_similarity_matrix(vvbool &spikes, string file_path, int delay, const string &similarity_matrix_type = "correlation");