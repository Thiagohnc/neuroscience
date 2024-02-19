#include "graph_generator.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "params.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main(void) { 
    const int N = param_N();
    const int T = param_T();

    bool** spike;
    double** fire_rate;
    double** M;

    spike = (bool**) malloc(sizeof(bool*) * N);
    fire_rate = (double**) malloc(sizeof(double*) * N);
    M = (double**) malloc(sizeof(double*) * N);

    for(int i = 0; i < N; i++) {
        spike[i] = (bool*) malloc(sizeof(bool) * T);
        fire_rate[i] = (double*) malloc(sizeof(double) * T);
        M[i] = (double*) malloc(sizeof(double) * T);
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
        for(int t = 0; t <= T; t++) {
            fire_rate[u][t] = g.kth_node(u).b();
            spike[u][t] = 0;
        }
    }
    
    double avg_w = 0;
    for(int u = 0; u < N; u++) {
        avg_w += (g.kth_node(u).W()) / N;
    }
    
    /* Simulation */
    for(int t = 0; t <= T; t++) {
        /* Calculating Fire Rate */
        for(int u = 0; u < N; u++) {
            for(int tt = max(0, t - 1); tt < t; tt++) {
                //cout << t << " " << tt << endl;
                for(int k = 0; k < g.neighbor_quantity(u); k++) {
                    int v = g.kth_neighbor(u,k);
                    double w = g.kth_weight(u,k);
                    //fire_rate[v][t] += w * spike[u][tt];
                    fire_rate[v][t] += (w * spike[u][tt]) / g.kth_node(u).W();
                    //fire_rate[v][t] += (w * spike[u][tt]) / avg_w;
                }
            }
        }
        
        /* Firing */
        for(int u = 0; u < N; u++) {
            double prob = logistic(fire_rate[u][t]);
            spike[u][t] = coin_flip(prob);
        }
    }
    
    for(int u = 0; u < N; u++) {
        for(int t = 0; t < T; t++) {
            //cout << fire_rate[u][t] << " ";
            cout << spike[u][t] << " ";
        }
        cout << endl;
    }
        
    /*double alpha = 0.8;
    for(int u = 0; u < N; u++) {
        cout << M[u][0] << " ";
        for(int t = 1; t <= T; t++) {
            M[u][t] = alpha * spike[u][t] + (1 - alpha) * M[u][t-1];
            
            M[u][t] = min(M[u][t], 1 - 1e-10);
            M[u][t] = max(M[u][t], 1e-10);
            
            //cout << M[u][t] << " ";
            cout << inverse_logistic(M[u][t]) << " ";
        }
        cout << endl;
    }*/
    
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
        cout << endl;
    }*/
    
    
    /*for(int u = 0; u < N; u++) {
        for(int v = 0; v < N; v++) {
            int tem = -1;
            for(int k = 0; k < g.neighbor_quantity(u); k++) {
                if(g.kth_neighbor(u,k) == v) tem = k;
            }
            cout << (tem == -1 ? 0 : g.kth_weight(u,tem)) << " ";
            //cout << (tem == -1 ? 0 : 1) << " ";
        }
        cout << endl;
    }*/
    
    return 0;
}