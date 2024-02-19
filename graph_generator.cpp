#include "graph_generator.hpp"
#include "utils.hpp"
#include "params.hpp"
#include <cassert>
#include <cmath>
#include <random>
#include <vector>

#include <iostream>

Graph stochastic_block_model(std::vector<std::vector<int> > &groups, std::vector<std::vector<double> > &p) {
    std::default_random_engine generator(param_seed());
    std::normal_distribution<double> intra(param_w_intra_mean(), param_w_intra_std());
    std::normal_distribution<double> inter(param_w_inter_mean(), param_w_inter_std());
    double intra_exc_portion = param_intra_exchitatory_portion();
    double inter_exc_portion = param_inter_exchitatory_portion();
    double b = inverse_logistic(param_auto_activ());
    
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
                        weight = intra(generator);
                        if(coin_flip(1 - intra_exc_portion))
                            weight *= -1;
                    }
                    else {
                        weight = inter(generator);
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

    //exit(0);

    //std::cout << "d" << std::endl;
    for(int i = 0; i < N; i++) {
        g.kth_node(i).set_b(b);
    }
    //std::cout << "e" << std::endl;
    
    //g.sort_all_edges();
    return g;
}
