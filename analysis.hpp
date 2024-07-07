#include <string>
#include <vector>

using namespace std;

double pearson(int a, int b, vector<vector<int>> &spikes, vector<double> &means, vector<double> &dens, vector<double> &delayed_dens, int delay);
vector<vector<double>> pearson_all_pairs(vector<vector<int>> &spikes, int delay);
vector<vector<int>> read_spike_trains_file(string file_path);
void write_pearson_correlation(vector<vector<int>> &spikes, string file_path);