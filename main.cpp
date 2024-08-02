#include "analysis.hpp"
#include "graph_generator.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "params.hpp"
#include <climits>
#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>

#define PRINT(str) if(!param_silent()) cout << str;
#define PRINTLN(str) if(!param_silent()) cout << str << endl;

using namespace std;

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
    
    if(argc > 1) 
        set_params_path(argv[1]);
    else
        set_params_path("params.txt");
    
    const int samples = param_samples();
    // Initial seed to set a different and random seed for each sample
    set_seed(param_seed());
    const vector<long unsigned int> samples_seeds = rand_vector(samples, ULONG_MAX);
    const int N = param_N();
    const int T = param_T();
    const int BURN_T = param_BURN_T();
	string path;
    
    for(int sample = 0; sample < samples; sample++) {
		PRINTLN("Amostra " << sample + 1 << " de " << samples << ":");
		
		/* Creating Output Folder */
        
        const string output_folder = param_output_folder() + "/" + to_string(sample + 1);
        mkdir_tree(output_folder);
		
		ifstream check_done_file;
		check_done_file.open(output_folder + "/done");
		if(check_done_file) {
			check_done_file.close();
			if(!param_silent()) PRINTLN("Amostra já calculada");
			continue;
		}
		
		/* Allocating Space */
		
        vector<vector<bool>> spike_trains(N);
        vector<double> firing_rate(N);
		
        set_seed(samples_seeds[sample]);
		
        for(int i = 0; i < N; i++) {
            spike_trains[i].resize(T+1);
			progress_bar(i + 1, N, "Reservando espaço para os vetores");
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
            for(int t = 0; t <= T ; t++)
                spike_trains[u][t] = false;
			firing_rate[u] = param_mu();
			progress_bar(u + 1, N, "Inicializando");
        }
        
        double avg_w = 0;
        for(int u = 0; u < N; u++) {
            avg_w += (g.kth_node(u).W()) / N;
        }
        
        /* Simulation */
		
        for(int t = 0; t <= (T + BURN_T); t++) {
            /* Initialization */
            for(int u = 0; u < N; u++) {
                firing_rate[u] = param_mu();
            }
            
            /* Activation by neighbors */
            for(int u = 0; u < N; u++) {
                for(int tt = max(0, t - 1); tt < t; tt++) {
					if(spike_trains[u][tt%(T+1)]) {
						for(int k = 0; k < g.neighbor_quantity(u); k++) {
							int v = g.kth_neighbor(u,k);
							double w = g.kth_weight(u,k);
							firing_rate[v] += (w * spike_trains[u][tt%(T+1)])/(double)N;
						}
					}
                }
            }
            
            /* Firing */
            for(int u = 0; u < N; u++) {
                spike_trains[u][t%(T+1)] = coin_flip(firing_rate[u]);
            }
			
			if(t % (T/100) == 0 || t == T + BURN_T) progress_bar(t, T + BURN_T, "Simulação");
        }
        
        /* Save current parameters in file */
        
        ifstream params_source("params.txt", ios::binary);
        ofstream params_dest(param_output_folder() + "/params", ios::binary);
        params_dest << params_source.rdbuf();
        
        /* Output Spike Trains */
        
		if(param_spike_trains_file()) {
			path = output_folder + "/spike_trains";
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
		
		/* Output Firing Rate */
        
		if(param_firing_rate_file()) {
			//path = output_folder + "/firing_rate";
			//ofstream firing_rate_file(path);
			//if(!firing_rate_file.is_open()) {PRINTLN("Unable to open file firing_rate"); exit(0);}
			//for(int u = 0; u < N; u++) {
			//	for(int t = BURN_T; t < (T + BURN_T); t++) {
			//		firing_rate_file << firing_rate[u][t%(T+1)] << " ";
			//	}
			//	firing_rate_file << '\n';
			//	progress_bar(u + 1, N, "Output: Firing Rate");
			//}
			//firing_rate_file.close();
			//
			//if(param_firing_rate_file() == 2)
			//	zip_and_remove(path);
			//
			PRINTLN("Firing_rate file generation is currently unavailable");
		}
        
        /* Output Adjacency 0/1 List */

		if(param_adjacency_0_1_file()) {
			path = output_folder + "/adjacency_0_1";
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
		
		/* Output Adjacency Weights List */
		
		if(param_adjacency_weights_file()) {
			path = output_folder + "/adjacency_weights";
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
		
		/* Output Pearson Correlation */
		path = output_folder + "/pearson";
		vector<bool> temp(T+1);
		for(int u = 0; u < N; u++) {
			for(int t = BURN_T; t <= (T + BURN_T); t++) temp[t-BURN_T] = spike_trains[u][t%(T+1)];
			for(int t = 0; t <= T; t++) spike_trains[u][t] = temp[t];
			progress_bar(u + 1, N, "Formatando matriz de spike trains");
		}
		write_pearson_correlation(spike_trains, path);
	
		/* Clearing Allocated Space */
		
		for(int i = 0; i < N; i++) {
            spike_trains[i].clear();
			progress_bar(i + 1, N, "Liberando espaço dos vetores");
        }
		spike_trains.clear();
		firing_rate.clear();
		
		/* Spectral Clustering */
		
		string cmd = "python3 analysis/spectral_clustering.py";
		cmd += " input_folder=" + output_folder;
		cmd += " n_clusters=2";
		cmd += " seed=" + to_string(samples_seeds[sample]);
		progress_bar(0, 2, "Spectral Clustering");
		system((cmd + " pot=1" + " filename=spectral_clustering_1").c_str());
		progress_bar(1, 2, "Spectral Clustering");
		system((cmd + " pot=2" + " filename=spectral_clustering_2").c_str());
		progress_bar(2, 2, "Spectral Clustering");
		
		/* Removing Pearson File if needed */
		path = output_folder + "/pearson";
		if(param_pearson_file() == 0)
			system(("rm " + path).c_str());
		if(param_pearson_file() == 2)
			zip_and_remove(path);
		
		/* "Done" file to notify that sample was fully calculated */
		ofstream done_file(output_folder + "/done");
		done_file.close();
    }
	
	/* Zip folder and remove */
	if(param_zip_results()) {
		system(("tar -czf " + param_output_folder() + ".tar.gz " + param_output_folder()).c_str());
		system(("rm -r " + param_output_folder()).c_str());
	}

    return 0;
}