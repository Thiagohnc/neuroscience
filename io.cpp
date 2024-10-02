#include "graph.hpp"
#include "io.hpp"
#include "params.hpp"
#include "utils.hpp"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace {
    class GenericIO {
    private:
        const string filename;
        const string filepath;
        ofstream filestream;
    public:
        GenericIO(const string &_filename, const string &_output_folder) :
        filename(_filename), filepath(_output_folder + '/' + _filename) {
            filestream = ofstream(filepath);
            if(!filestream.is_open()) {PRINTLN("Unable to open file " + filename); exit(1);}
        }

        ~GenericIO() {
            if(filestream.is_open()) filestream.close();
        }

        template<typename T>
        GenericIO& operator<<(T x) {
            filestream << x;
            return *this;
        }

        void next(int current, int total) {
            filestream << '\n';
            progress_bar(current + 1, total, "Output: " + filename);
        }

        void finish() {
            filestream.close();
            if(stoi(get_param(filename + "_file")) == 2)
                zip_and_remove(filepath);
        }
    };
}

void output_spike_trains(const vector<vector<bool>> &spike_trains, const string &output_folder) {
    int N = param_N();
    int BURN_T = param_BURN_T(), T = param_T();
    GenericIO io("spike_trains", output_folder);
    for(int u = 0; u < N; u++) {
        for(int t = BURN_T; t < (T + BURN_T); t++)
            io << (spike_trains[u][t%(T+1)] ? 1 : 0) << " ";
        io.next(u, N);
    }
    io.finish();
}

void output_spike_average(const vector<vector<bool>> &spike_trains, const string &output_folder) {
    int N = param_N();
    int BURN_T = param_BURN_T(), T = param_T();
    GenericIO io("spike_average", output_folder);
    for(int u = 0; u < N; u++) {
        int total = 0;
        for(int t = BURN_T; t < (T + BURN_T); t++)
            total += spike_trains[u][t%(T+1)];
        io << (double)total/T;
        io.next(u, N);
    }
    io.finish();
}

void output_spike_variance(const vector<vector<bool>> &spike_trains, const string &output_folder) {
    int N = param_N();
    int BURN_T = param_BURN_T(), T = param_T();
    GenericIO io("spike_variance", output_folder);
    for(int u = 0; u < N; u++) {
        double variance_num = 0;
        const double spike_avg = mean(spike_trains[u]);
        for(int t = BURN_T; t < (T + BURN_T); t++) 
            variance_num += (spike_trains[u][t%(T+1)] - spike_avg) * (spike_trains[u][t%(T+1)] - spike_avg);
        io << (double)variance_num/T;
        io.next(u, N);
    }
    io.finish();
}

void output_adjacency_0_1(Graph &g, const string &output_folder) {
    int N = param_N();
    GenericIO io("adjacency_0_1", output_folder);
    for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            int connection = -1;
            for(int k = 0; k < g.neighbor_quantity(u); k++) {
                if(g.kth_neighbor(u,k) == v) {
                    connection = k;
                    break;
                }
            }
            io << (connection == -1 ? 0 : 1) << " ";
        }
        io.next(u, N);
    }
    io.finish();
}

void output_adjacency_weights(Graph &g, const string &output_folder) {
    int N = param_N();
    GenericIO io("adjacency_weights", output_folder);
    for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            int connection = -1;
            for(int k = 0; k < g.neighbor_quantity(u); k++) {
                if(g.kth_neighbor(u,k) == v) {
                    connection = k;
                    break;
                }
            }
            io << (connection == -1 ? 0 : g.kth_weight(u,connection)) << " ";
        }
        io.next(u, N);
    }
    io.finish();
}