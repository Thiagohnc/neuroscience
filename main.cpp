#include "analysis.hpp"
#include "graph_generator.hpp"
#include "graph.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "params.hpp"
#include <climits>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
    
	string params_filename = "params.txt";
    if(argc > 1) {
        params_filename = argv[1];
	}
	set_params_path(params_filename);
    
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
        
        ifstream params_source(params_filename);
		stringstream params_text;
		params_text << params_source.rdbuf();
        ofstream params_dest(param_output_folder() + "/params");
        params_dest << regex_replace(params_text.str(), regex("seed=auto"), "seed=" + to_string(param_seed()));
        
        /* Outputs */
        
		if(param_spike_trains_file())		output_spike_trains(spike_trains, output_folder);
		if(param_spike_average_file()) 		output_spike_average(spike_trains, output_folder);
		if(param_spike_variance_file()) 	output_spike_variance(spike_trains, output_folder);
		if(param_firing_rate_file())		PRINTLN("Firing_rate file generation is currently unavailable");
		if(param_adjacency_0_1_file())		output_adjacency_0_1(g, output_folder);
		if(param_adjacency_weights_file())	output_adjacency_weights(g, output_folder);
		
		/* Pearson Correlation */
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
		exec_shell(cmd + " pot=1" + " filename=spectral_clustering_1");
		progress_bar(1, 2, "Spectral Clustering");
		exec_shell(cmd + " pot=2" + " filename=spectral_clustering_2");
		progress_bar(2, 2, "Spectral Clustering");
		
		/* Removing Pearson File if needed */
		path = output_folder + "/pearson";
		if(param_pearson_file() == 0)
			exec_shell("rm " + path);
		if(param_pearson_file() == 2)
			zip_and_remove(path);
		
		/* "Done" file to notify that sample was fully calculated */
		ofstream done_file(output_folder + "/done");
		done_file.close();
    }
	
	/* Zip folder and remove */
	if(param_zip_results()) {
		exec_shell("tar -czf " + param_output_folder() + ".tar.gz " + param_output_folder());
		exec_shell("rm -r " + param_output_folder());
	}

    return 0;
}