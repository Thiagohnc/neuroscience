#include "graph_generator.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "params.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>

using namespace std;

int main(int argc, char *argv[]) {
    
    if(argc > 1) 
        set_params_path(argv[1]);
    else
        set_params_path("params.txt");
    
    const int samples = param_samples();
    // Initial seed to set a different and random seed for each sample
    set_seed(param_seed());
    const vector<int> samples_seeds = rand_vector(samples, INT8_MAX);
    const int N = param_N();
    const int T = param_T();
    const int BURN_T = param_BURN_T();
    
    for(int sample = 0; sample < samples; sample++) {  
        set_seed(samples_seeds[sample]);

        bool** spike;
        double** fire_rate;

        spike = (bool**) malloc(sizeof(bool*) * N);
        fire_rate = (double**) malloc(sizeof(double*) * N);

        for(int i = 0; i < N; i++) {
            spike[i] = (bool*) malloc(sizeof(bool) * (T + 1));
            fire_rate[i] = (double*) malloc(sizeof(double) * (T + 1));
        }

        /* Graph Creation */
        
        Graph g;
        
        vector<vector<int> > groups;
        groups.resize(2);
        
        for(int i = 0; i < N/2; i++) groups[0].push_back(i);
        for(int i = N/2; i < N; i++) groups[1].push_back(i);
        
        vector<vector<double> > p;
        p.push_back(vector<double>({param_p(), param_q()}));
        p.push_back(vector<double>({param_q(), param_p()}));
        
        g = stochastic_block_model(groups, p);
        
        /* Initialization */
        
        for(int u = 0; u < N; u++) {
            for(int t = 0; t <= T ; t++) {
                fire_rate[u][t] = g.kth_node(u).b();
                spike[u][t] = 0;
            }
        }
        
        double avg_w = 0;
        for(int u = 0; u < N; u++) {
            avg_w += (g.kth_node(u).W()) / N;
        }
        
        /* Simulation */
		
        for(int t = 0; t <= (T + BURN_T); t++) {
            /* Initialization */
            for(int u = 0; u < N; u++) {
                fire_rate[u][t%(T+1)] = param_mu();
            }
            
            /* Activation by neighbors */
            for(int u = 0; u < N; u++) {
                for(int tt = max(0, t - 1); tt < t; tt++) {
                    for(int k = 0; k < g.neighbor_quantity(u); k++) {
                        int v = g.kth_neighbor(u,k);
                        int w = g.kth_weight(u,k);
                        fire_rate[v][t%(T+1)] += (w * spike[u][tt%(T+1)])/(double)N;
                    }
                }
            }
            
            /* Firing */
            for(int u = 0; u < N; u++) {
                spike[u][t%(T+1)] = coin_flip(fire_rate[u][t%(T+1)]);
            }
        }
        
        /* Creating Output Folder */
        
        const string output_folder = param_output_folder() + "/" + to_string(sample + 1);
        mkdir_tree(output_folder);
        
        /* Save current parameters in file */
        
        ifstream params_source("params.txt", ios::binary);
        ofstream params_dest(param_output_folder() + "/params", ios::binary);
        params_dest << params_source.rdbuf();
        
        /* Output Spike Trains */
        
		if(param_spike_trains_file()) {
			ofstream spike_trains_file(output_folder + "/spike_trains");
			if(!spike_trains_file.is_open()) {cout << "Unable to open file spike_trains" << '\n'; exit(0);}
			for(int u = 0; u < N; u++) {
				for(int t = BURN_T; t < (T + BURN_T); t++) {
					spike_trains_file << spike[u][t%(T+1)] << " ";
				}
				spike_trains_file << '\n';
			}
			spike_trains_file.close();
		}
		
		/* Output Firing Rate */
        
		if(param_firing_rate_file()) {
			ofstream firing_rate_file(output_folder + "/firing_rate");
			if(!firing_rate_file.is_open()) {cout << "Unable to open file firing_rate" << '\n'; exit(0);}
			for(int u = 0; u < N; u++) {
				for(int t = BURN_T; t < (T + BURN_T); t++) {
					firing_rate_file << fire_rate[u][t%(T+1)] << " ";
				}
				firing_rate_file << '\n';
			}
			firing_rate_file.close();
		}
        
        /* Output Adjacency 0/1 List */

		if(param_adjacency_0_1_file()) {
			ofstream adj_file(output_folder + "/adjacency_0_1");
			if(!adj_file.is_open()) {cout << "Unable to open file adjacency_0_1" << '\n'; exit(0);}
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
				adj_file << '\n';
			}
			adj_file.close();
        }
		
		if(param_adjacency_weights_file()) {
			ofstream adj_w_file(output_folder + "/adjacency_weights");
			if(!adj_w_file.is_open()) {cout << "Unable to open file adjacency_weights" << '\n'; exit(0);}
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
			}
			adj_w_file.close();
		}
    }

    
    return 0;
}