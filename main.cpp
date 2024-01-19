#include "graph_generator.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "params.hpp"
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

const int N = 300;
const int T = 1000;

bool spike[N][T];
double fire_rate[N][T];

int main(void) { 
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
        for(int t = 0; t < T; t++) {
            fire_rate[u][t] = g.kth_node(u).b();
        }
    }
    
    /* Simulation */
    
    for(int t = 0; t < T; t++) {
        /* Calculating Fire Rate */
        for(int u = 0; u < N; u++) {
            for(int tt = max(0, t - 1); tt < t; tt++) {
                for(int k = 0; k < g.neighbor_quantity(u); k++) {
                    int v = g.kth_neighbor(u,k);
                    double w = g.kth_weight(u,k);
                    fire_rate[v][t] += w * spike[u][tt];
                }
            }
        }
        
        /* Firing */
        for(int u = 0; u < N; u++) {
            double prob = exp(fire_rate[u][t]) / (1 + exp(fire_rate[u][t]));
            spike[u][t] = coin_flip(prob);
        }
    }
    
    for(int u = 0; u < N; u++) {
        for(int t = 0; t < T; t++) {
            cout << fire_rate[u][t] << " ";
        }
        cout << endl;
    }
    
    
    /*for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            int tem = -1;
            for(int k = 0; k < g.neighbor_quantity(u); k++) {
                if(g.kth_neighbor(u,k) == v) tem = k;
            }
            //cout << (tem == -1 ? 0 : g.kth_weight(u,tem)) << " ";
            cout << (tem == -1 ? 0 : 1) << " ";
        }
        cout << endl;
    }*/
    
    return 0;
}