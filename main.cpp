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
        double** M;

        spike = (bool**) malloc(sizeof(bool*) * N);
        fire_rate = (double**) malloc(sizeof(double*) * N);
        M = (double**) malloc(sizeof(double*) * N);

        for(int i = 0; i < N; i++) {
            spike[i] = (bool*) malloc(sizeof(bool) * (T + BURN_T + 1));
            fire_rate[i] = (double*) malloc(sizeof(double) * (T + BURN_T + 1));
            M[i] = (double*) malloc(sizeof(double) * (T + BURN_T + 1));
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
            M[u][0] = 0.01;
            for(int t = 0; t <= (T + BURN_T); t++) {
                fire_rate[u][t] = g.kth_node(u).b();
                spike[u][t] = 0;
            }
        }
        
        double avg_w = 0;
        for(int u = 0; u < N; u++) {
            avg_w += (g.kth_node(u).W()) / N;
        }
        
        /* Simulation - Giulio & Guilherme */
        for(int t = 0; t <= (T + BURN_T); t++) {
            /* Initialization */
            for(int u = 0; u < N; u++) {
                fire_rate[u][t] = param_mu();
            }
            
            /* Activation by neighbors */
            for(int u = 0; u < N; u++) {
                for(int tt = max(0, t - 1); tt < t; tt++) {
                    for(int k = 0; k < g.neighbor_quantity(u); k++) {
                        int v = g.kth_neighbor(u,k);
                        int w = g.kth_weight(u,k);
                        fire_rate[v][t] += (w * spike[u][tt])/(double)N;
                    }
                }
            }
            
            /* Firing */
            for(int u = 0; u < N; u++) {
                spike[u][t] = coin_flip(fire_rate[u][t]);
            }
        }
        
        /* Simulation - Daniel */
        //double pw = param_pw();
        //double auto_activ = param_auto_activ();
        //for(int t = 0; t <= T; t++) {
        //    /* Activation by neighbors */
        //    for(int u = 0; u < N; u++) {
        //        for(int tt = max(0, t - 1); tt < t; tt++) {
        //            for(int k = 0; k < g.neighbor_quantity(u); k++) {
        //                int v = g.kth_neighbor(u,k);
        //                spike[v][t] |= (spike[u][tt] && coin_flip(pw));
        //            }
        //        }
        //    }
        //    
        //    /* Auto activation */
        //    for(int u = 0; u < N; u++) {
        //        spike[u][t] |= coin_flip(auto_activ);
        //    }
        //}
        
        /* Simulation */
        //for(int t = 0; t <= T; t++) {
        //    /* Calculating Fire Rate */
        //    for(int u = 0; u < N; u++) {
        //        for(int tt = max(0, t - 1); tt < t; tt++) {
        //            //cout << t << " " << tt << '\n';
        //            for(int k = 0; k < g.neighbor_quantity(u); k++) {
        //                int v = g.kth_neighbor(u,k);
        //                double w = g.kth_weight(u,k);
        //                //fire_rate[v][t] += w * spike[u][tt];
        //                fire_rate[v][t] += (w * spike[u][tt]) / N;
        //                //fire_rate[v][t] += (w * spike[u][tt]) / g.kth_node(u).W();
        //                //fire_rate[v][t] += (w * spike[u][tt]) / avg_w;
        //            }
        //        }
        //    }
        //    
        //    /* Firing */
        //    for(int u = 0; u < N; u++) {
        //        double prob = logistic(fire_rate[u][t]);
        //        spike[u][t] = coin_flip(prob);
        //    }
        //}
        
        /* Creating Output Folder */
        
        const string output_folder = param_output_folder() + "/" + to_string(sample + 1);
        mkdir_tree(output_folder);
        
        /* Save current parameters in file */
        
        ifstream params_source("params.txt", ios::binary);
        ofstream params_dest(param_output_folder() + "/params", ios::binary);
        params_dest << params_source.rdbuf();
        
        /* Output Spike Trains & Firing Rate */
        
        ofstream spike_trains_file(output_folder + "/spike_trains");
        ofstream firing_rate_file(output_folder + "/firing_rate");
        
        if(!spike_trains_file.is_open()) {cout << "Unable to open file spike_trains" << '\n'; exit(0);}
        if(!firing_rate_file.is_open()) {cout << "Unable to open file firing_rate" << '\n'; exit(0);}
        
        for(int u = 0; u < N; u++) {
            for(int t = BURN_T; t < (T + BURN_T); t++) {
                spike_trains_file << spike[u][t] << " ";
                firing_rate_file << fire_rate[u][t] << " ";
            }
            spike_trains_file << '\n';
            firing_rate_file << '\n';
        }
        
        spike_trains_file.close();
        firing_rate_file.close();
        
        /* Output EWMA */
        
        // ofstream ewma_file(output_folder + "/ewma");
        // 
        // if(!ewma_file.is_open()) {cout << "Unable to open file ewma" << '\n'; exit(0);}
        // 
        // double alpha = param_alpha_ewma();
        // for(int u = 0; u < N; u++) {
        //     ewma_file << M[u][0] << " ";
        //     for(int t = BURN_T; t <= (T + BURN_T); t++) {
        //         M[u][t] = alpha * spike[u][t] + (1 - alpha) * M[u][t-1];
        //         
        //         M[u][t] = min(M[u][t], 1 - 1e-10);
        //         M[u][t] = max(M[u][t], 1e-10);
		// 
        //         ewma_file << inverse_logistic(M[u][t]) << " ";
        //     }
        //     ewma_file << '\n';
        // }
        // 
        // ewma_file.close();
        
        /*int window = 50;
        for(int u = 0; u < N; u++) {
            for(int t = 0; t < T; t++) {
                double val = 0;
                for(int tt = max(0,t-window); tt < min(T,t+window+1); tt++) {
                    val += spike[u][tt];
                }
                val /= (double)(2 * window + 1);
                
                val = min(val, 1 - 1e-10);
                val = max(val, 1e-10);
                
                cout << inverse_logistic(val) << " ";
            }
            cout << '\n';
        }*/
        
        
        /* Output Adjacency List */

        ofstream adj_file(output_folder + "/adjacency_0_1");
        ofstream adj_w_file(output_folder + "/adjacency_weights");
        
        if(!adj_file.is_open()) {cout << "Unable to open file adjacency_0_1" << '\n'; exit(0);}
        if(!adj_w_file.is_open()) {cout << "Unable to open file adjacency_weights" << '\n'; exit(0);}
        
        for(int u = 0; u < N; u++) {
            for(int v = 0; v < N; v++) {
                int tem = -1;
                for(int k = 0; k < g.neighbor_quantity(u); k++) {
                    if(g.kth_neighbor(u,k) == v) tem = k;
                }
                adj_file << (tem == -1 ? 0 : 1) << " ";
                adj_w_file << (tem == -1 ? 0 : g.kth_weight(u,tem)) << " ";
            }
            adj_file << '\n';
            adj_w_file << '\n';
        }
        
        adj_file.close();
    }

    
    return 0;
}