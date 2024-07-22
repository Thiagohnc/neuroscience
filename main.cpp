#include "analysis.hpp"
#include "graph_generator.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "params.hpp"
//#include <boost/iostreams/copy.hpp>
//#include <boost/iostreams/filtering_stream.hpp>
//#include <boost/iostreams/filter/gzip.hpp>
//#include <boost/iostreams/filter/zlib.hpp>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <sys/stat.h>

using namespace std;
//namespace io = boost::iostreams;

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
    
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
		cout << "Amostra " << sample + 1 << " de " << samples << ":" << endl;
		
		set_seed(samples_seeds[sample]);
		
		/* Creating Output Folder */
        
        const string output_folder = param_output_folder() + "/" + to_string(sample + 1);
        mkdir_tree(output_folder);
		
		/* Checking if sample was already calculated */
		
		ifstream check_done_file;
		check_done_file.open(output_folder + "/done");
		if(check_done_file) {
			check_done_file.close();
			cout << "Amostra já calculada" << endl;
			continue;
		}
		
		/* Allocating Space */

        vector<vector<int>> spike_trains(N);
        vector<vector<double>> firing_rate(N);

        for(int i = 0; i < N; i++) {
            spike_trains[i].resize(T+1);
            firing_rate[i].resize(T+1);
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
            for(int t = 0; t <= T ; t++) {
                firing_rate[u][t] = g.kth_node(u).b();
                spike_trains[u][t] = 0;
            }
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
                firing_rate[u][t%(T+1)] = param_mu();
            }
            
            /* Activation by neighbors */
            for(int u = 0; u < N; u++) {
                for(int tt = max(0, t - 1); tt < t; tt++) {
                    for(int k = 0; k < g.neighbor_quantity(u); k++) {
                        int v = g.kth_neighbor(u,k);
                        int w = g.kth_weight(u,k);
                        firing_rate[v][t%(T+1)] += (w * spike_trains[u][tt%(T+1)])/(double)N;
                    }
                }
            }
            
            /* Firing */
            for(int u = 0; u < N; u++) {
                spike_trains[u][t%(T+1)] = coin_flip(firing_rate[u][t%(T+1)]);
            }
			
			if(t % 10000 == 0 || t == T + BURN_T + 1) progress_bar(t + 1, T + BURN_T + 1, "Simulação");
        }
        
        /* Save current parameters in file */
        
        ifstream params_source("params.txt", ios::binary);
        ofstream params_dest(param_output_folder() + "/params", ios::binary);
		string params_str((istreambuf_iterator<char>(params_source)), istreambuf_iterator<char>());
		regex match("seed=auto");
		string repl = "seed=" + to_string(param_seed()) + "(auto)";
        string params_to_write = regex_replace(params_str, match, repl);
		params_dest << params_to_write;
        
        /* Output Spike Trains */
        
		if(param_spike_trains_file()) {
			//     ofstream spike_trains_file("/spike_trains.gz", ofstream::binary);
			//     boost::iostreams::filtering_streambuf< boost::iostreams::input> in;
			//     in.push( boost::iostreams::gzip_compressor());
			//     stringstream spike_trains_data;
			
			// compress
			//vector<char> packed;
			//io::filtering_ostream out;
			//out.push(io::gzip_compressor(io::gzip_params(io::gzip::best_compression)));
			//out.push(io::back_inserter(packed));
			//out.write(reinterpret_cast<const char*>(&wdata[0]), wdata.length() * sizeof(wchar_t));
			//io::close(out);
			ofstream spike_trains_file(output_folder + "/spike_trains");
			if(!spike_trains_file.is_open()) {cout << "Unable to open file spike_trains" << '\n'; exit(0);}
			for(int u = 0; u < N; u++) {
				for(int t = BURN_T; t < (T + BURN_T); t++) {
					spike_trains_file << spike_trains[u][t%(T+1)] << " ";
				}
				spike_trains_file << '\n';
			}
			spike_trains_file.close();
			//     in.push(spike_trains_data);
			//     boost::iostreams::copy(in, spike_trains_file);
		}
		
		/* Output Firing Rate */
        
		if(param_firing_rate_file()) {
			ofstream firing_rate_file(output_folder + "/firing_rate");
			if(!firing_rate_file.is_open()) {cout << "Unable to open file firing_rate" << '\n'; exit(0);}
			for(int u = 0; u < N; u++) {
				for(int t = BURN_T; t < (T + BURN_T); t++) {
					firing_rate_file << firing_rate[u][t%(T+1)] << " ";
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
		
		/* Output Adjacency Weights List */
		
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
		
		/* Output Pearson Correlation */
		
		if(param_pearson_file()) {
			vector<int> temp(T+1);
			for(int u = 0; u < N; u++) {
				for(int t = BURN_T; t <= (T + BURN_T); t++) temp[t-BURN_T] = spike_trains[u][t%(T+1)];
				for(int t = 0; t <= T; t++) spike_trains[u][t] = temp[t];
				progress_bar(u + 1, N, "Formatando matriz de spike trains");
			}
			write_pearson_correlation(spike_trains, output_folder + "/pearson");
		}
		
		/* "Done" file to notify that sample was fully calculated */
		ofstream done_file(output_folder + "/done");
		done_file.close();
		
		/* Clearing Allocated Space */
		
		for(int i = 0; i < N; i++) {
            spike_trains[i].clear();
            firing_rate[i].clear();
			progress_bar(i + 1, N, "Liberando espaço dos vetores");
        }
		spike_trains.clear();
		firing_rate.clear();
    }

    
    return 0;
}