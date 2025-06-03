#include "graph_generator.hpp"
#include "graph.hpp"
#include "params.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <random>
#include <string>

#include <iostream>
#include <stdexcept>

using namespace std;

Graph stochastic_block_model(vvint &groups, vvdouble &p) {
    default_random_engine generator(param_seed());
    double intra_exc_portion = param_intra_exchitatory_portion();
    double inter_exc_portion = param_inter_exchitatory_portion();
    
    int N = 0;
    
    assert(groups.size() == p.size());
    for(int g = 0; g < (int)p.size(); g++)
        assert(groups.size() == p[g].size());
    
    
    for(int g = 0; g < (int)groups.size(); g++)
        N += groups[g].size();
    
    Graph g = Graph(N);
    for(int g_orig = 0; g_orig < (int)groups.size(); g_orig++) {
        for(int g_dest = 0; g_dest < (int)groups.size(); g_dest++) {
            double prob = p[g_orig][g_dest];
            
            for(int i = 0; i < (int)groups[g_orig].size(); i++) {
                for(int j = 0; j < (int)groups[g_dest].size(); j++) {
                    int o = groups[g_orig][i];
                    int d = groups[g_dest][j];
                    double weight;
                    
                    if(o == d) continue;
                    
                    if(g_orig == g_dest) {
                        weight = param_mu_in();
                        if(coin_flip(1 - intra_exc_portion))
                            weight *= -1;
                    }
                    else {
                        weight = param_mu_out();
                        if(coin_flip(1 - inter_exc_portion))
                            weight *= -1;
                    }
                    
                    if(coin_flip(prob)) {
                        g.add_edge(o, d, weight);
                    }
                }
            }
        }
    }

    return g;
}

Graph sc_stochastic_block_model(vvint &groups, vvdouble &p, int max_tries) {
    for(int i = 0; i < max_tries; i++) {
        Graph g = stochastic_block_model(groups, p);
        if(strongly_connected(g))
            return g;
        PRINTLN(i << " Generated graph was not strongly connected");
    }
    throw runtime_error("could not generate a strongly connected graph");
} 

Graph graph_from_file(string filepath) {
    vvdouble adj_matrix;
    ifstream file(filepath);
    if(file.is_open()) {
        string line;
        int u = 0;
        while(getline(file, line)) {
            line = strip(line);
            if(line.size() == 0 || line[0] == '#') continue;
            adj_matrix.push_back(vector_to_doubles(split(line, ' ')));
            u++;
        }
    }

    const int N = (int) adj_matrix.size();

    for(int i = 0; i < (int) adj_matrix.size(); i++)
        assert((int) adj_matrix[i].size() == N);

    Graph g(N);

    for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            if(adj_matrix[u][v] != 0)
                g.add_edge(u, v, adj_matrix[u][v]);
        }
    }

    return g;
}
