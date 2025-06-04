#include "analysis.hpp"
#include "graph_generator.hpp"
#include "graph.hpp"
#include "io.hpp"
#include "params.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <climits>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

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
    const vlonguint samples_seeds = rand_vector(samples, ULONG_MAX);
	const int T = param_T();
    const int BURN_T = param_BURN_T();
	double lambda = param_lambda();
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

		/* Graph Creation */
        
        Graph g;
        
		if(param_graph() == "SBM") {
			vvint groups;
			vint group_n = param_group_n();
			groups.resize(group_n.size());

			int u = 0;
			for(int i = 0; i < (int) groups.size(); i++)
				for(int k = 0; k < group_n[i]; k++)
					groups[i].push_back(u++);

			vvdouble p = param_p();

			if(param_should_be_strongly_connected())
				g = sc_stochastic_block_model(groups, p, 1000000);
			else
				g = stochastic_block_model(groups, p);
		}
		else {
			g = graph_from_file(param_graph());
		}

		const int N = g.N();

		/* Allocating Space */
		
        vvbool spike_trains(N);
        vdouble firing_rate(N);
		
        set_seed(samples_seeds[sample]);
		
        for(int i = 0; i < N; i++) {
            spike_trains[i].resize(T+1);
			progress_bar(i + 1, N, "Reservando espaço para os vetores");
        }


        
        /* Initialization */
        
        for(int u = 0; u < N; u++) {
            for(int t = 0; t <= T ; t++)
                spike_trains[u][t] = false;
			firing_rate[u] = lambda;
			progress_bar(u + 1, N, "Inicializando");
        }
        
        /* Simulation */
		
        for(int t = 0; t <= (T + BURN_T); t++) {
            /* Initialization */
            for(int u = 0; u < N; u++) {
                firing_rate[u] = lambda;
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
			
			if(T + BURN_T < 100 || t % ((T + BURN_T)/100) == 0 || t == T + BURN_T) progress_bar(t, T + BURN_T, "Simulação");
        }
        
        /* Save current parameters in file */
        
        ifstream params_source(params_filename);
		stringstream params_text;
		params_text << params_source.rdbuf();
        ofstream params_dest(param_output_folder() + "/params");
        params_dest << regex_replace(params_text.str(), regex("seed=auto"), "seed=" + to_string(param_seed()));
        
        /* Outputs */

		if(param_spike_trains_file())		output_spike_trains(spike_trains, output_folder);
		if(param_firing_rate_file())		output_firing_rate(g, spike_trains, output_folder);
		if(param_spike_average_file()) 		output_spike_average(spike_trains, output_folder);
		if(param_spike_variance_file()) 	output_spike_variance(spike_trains, output_folder);
		if(param_adjacency_0_1_file())		output_adjacency_0_1(g, output_folder);
		if(param_adjacency_weights_file())	output_adjacency_weights(g, output_folder);
		
		/* Pearson Correlation */
		path = output_folder + "/pearson";
		vbool temp(T+1);
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
		cmd += " n_clusters=" + to_string(param_group_n().size());
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