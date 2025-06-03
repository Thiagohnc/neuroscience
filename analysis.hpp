#include "types.hpp"
#include <string>

using namespace std;

double pearson(int a, int b, vvbool &spikes, vdouble &means, vdouble &dens, vdouble &delayed_dens);
vvdouble pearson_all_pairs(vvbool &spikes);
vvbool read_spike_trains_file(string file_path);
void write_pearson_correlation(vvbool &spikes, string file_path);