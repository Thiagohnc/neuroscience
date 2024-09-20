#include "graph.hpp"
#include "io.hpp"
#include "params.hpp"
#include "utils.hpp"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void output_spike_trains(const vector<vector<bool>> &spike_trains, const string &output_folder) {
    int N = param_N();
    int BURN_T = param_BURN_T(), T = param_T();
    string path = output_folder + "/spike_trains";
    ofstream spike_trains_file(path);
    if(!spike_trains_file.is_open()) {PRINTLN("Unable to open file spike_trains"); exit(0);}
    for(int u = 0; u < N; u++) {
        for(int t = BURN_T; t < (T + BURN_T); t++) {
            spike_trains_file << (spike_trains[u][t%(T+1)] ? 1 : 0) << " ";
        }
        spike_trains_file << '\n';
        progress_bar(u + 1, N, "Output: Spike Trains");
    }
    spike_trains_file.close();
    
    if(param_spike_trains_file() == 2)
        zip_and_remove(path);
}

void output_spike_average(const vector<vector<bool>> &spike_trains, const string &output_folder) {
    int N = param_N();
    int BURN_T = param_BURN_T(), T = param_T();
    string path = output_folder + "/spike_average";
    ofstream spike_average_file(path);
    if(!spike_average_file.is_open()) {PRINTLN("Unable to open file spike_average"); exit(0);}
    for(int u = 0; u < N; u++) {
        int total = 0;
        for(int t = BURN_T; t < (T + BURN_T); t++) {
            total += spike_trains[u][t%(T+1)];
        }
        spike_average_file << (double)total/T << '\n';
        progress_bar(u + 1, N, "Output: Spike Average");
    }
    spike_average_file.close();
    
    if(param_spike_average_file() == 2)
        zip_and_remove(path);
}

void output_spike_variance(const vector<vector<bool>> &spike_trains, const string &output_folder) {
    int N = param_N();
    int BURN_T = param_BURN_T(), T = param_T();
    string path = output_folder + "/spike_variance";
    ofstream spike_variance_file(path);
    if(!spike_variance_file.is_open()) {PRINTLN("Unable to open file spike_variance"); exit(0);}
    for(int u = 0; u < N; u++) {
        double variance_num = 0;
        const double spike_avg = mean(spike_trains[u]);
        for(int t = BURN_T; t < (T + BURN_T); t++) {
            variance_num += (spike_trains[u][t%(T+1)] - spike_avg) * (spike_trains[u][t%(T+1)] - spike_avg);
        }
        spike_variance_file << (double)variance_num/T << '\n';
        progress_bar(u + 1, N, "Output: Spike Variance");
    }
    spike_variance_file.close();
    
    if(param_spike_variance_file() == 2)
        zip_and_remove(path);
}

void output_adjacency_0_1(Graph &g, const string &output_folder) {
    int N = param_N();
    string path = output_folder + "/adjacency_0_1";
    ofstream adj_file(path);
    if(!adj_file.is_open()) {PRINTLN("Unable to open file adjacency_0_1"); exit(0);}
    for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            int connection = -1;
            for(int k = 0; k < g.neighbor_quantity(u); k++) {
                if(g.kth_neighbor(u,k) == v) {
                    connection = k;
                    break;
                }
            }
            adj_file << (connection == -1 ? 0 : 1) << " ";
        }
        progress_bar(u + 1, N, "Output: Adjacency 0/1");
        adj_file << '\n';
    }
    adj_file.close();
    
    if(param_adjacency_0_1_file() == 2)
        zip_and_remove(path);
}

void output_adjacency_weights(Graph &g, const string &output_folder) {
    int N = param_N();
    string path = output_folder + "/adjacency_weights";
    ofstream adj_w_file(path);
    if(!adj_w_file.is_open()) {PRINTLN("Unable to open file adjacency_weights"); exit(0);}
    for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            int connection = -1;
            for(int k = 0; k < g.neighbor_quantity(u); k++) {
                if(g.kth_neighbor(u,k) == v) {
                    connection = k;
                    break;
                }
            }
            adj_w_file << (connection == -1 ? 0 : g.kth_weight(u,connection)) << " ";
        }
        adj_w_file << '\n';
        progress_bar(u + 1, N, "Output: Adjacency Weights");
    }
    adj_w_file.close();
    
    if(param_adjacency_weights_file() == 2)
        zip_and_remove(path);
}