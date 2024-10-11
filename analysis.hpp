#include <string>
#include <vector>

using namespace std;

double pearson(int a, int b, vector<vector<bool>> &spikes, vector<double> &means, vector<double> &dens, vector<double> &delayed_dens);
vector<vector<double>> pearson_all_pairs(vector<vector<bool>> &spikes);
vector<vector<bool>> read_spike_trains_file(string file_path);
void write_pearson_correlation(vector<vector<bool>> &spikes, string file_path);