#include "graph.hpp"
#include "io.hpp"
#include "params.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <fstream>
#include <string>

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

void output_spike_trains(const vvbool &spike_trains, const string &output_folder) {
    int N = (int) spike_trains.size();
    int BURN_T = param_BURN_T(), T = param_T();
    GenericIO io("spike_trains", output_folder);
    for(int u = 0; u < N; u++) {
        for(int t = BURN_T; t < (T + BURN_T); t++)
            io << (spike_trains[u][t%(T+1)] ? 1 : 0) << " ";
        io.next(u, N);
    }
    io.finish();
}

void output_firing_rate(const Graph &g, const vvbool &spike_trains, const string &output_folder) {
    int N = (int) spike_trains.size();
    int BURN_T = param_BURN_T(), T = param_T();
    GenericIO io("firing_rate", output_folder);
    Graph rev = g.reversed_graph();
    double lambda = param_lambda();

    for(int u = 0; u < N; u++) {
        for(int t = 0; t < T + BURN_T; t++) {
            double firing_rate = lambda;
            for(int tt = max(0, t - 1); tt < t; tt++) {
                if(tt >= 0) {
                    for(int k = 0; k < rev.neighbor_quantity(u); k++) {
                        int v = rev.kth_neighbor(u, k);
                        double w = rev.kth_weight(u, k);
                        firing_rate += (w * spike_trains[v][tt%(T+1)])/(double)N;
                    }
                }
            }
            io << min(1.,max(0.,firing_rate)) << " ";
        }
        io.next(u, N);
    }
    io.finish();
}

void output_spike_average(const vvbool &spike_trains, const string &output_folder) {
    int N = (int) spike_trains.size();
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

void output_spike_variance(const vvbool &spike_trains, const string &output_folder) {
    int N = (int) spike_trains.size();
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

void output_adjacency_0_1(const Graph &g, const string &output_folder) {
    int N = g.N();
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

void output_adjacency_weights(const Graph &g, const string &output_folder) {
    int N = g.N();
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