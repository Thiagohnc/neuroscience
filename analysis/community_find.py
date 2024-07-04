import networkx as nx
import numpy as np
from cdlib import algorithms
#import leidenalg

def build_graph(input_file):
    _adj = []

    #with open(f'output_alpha_{alpha}.txt') as file:
    #with open(f'output_p_{p}.txt') as file:
    with open(input_file) as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split(' ')
            if len(line) > 0:
                _adj.append([])
            for j in range(len(line)):
                _adj[-1].append(float(line[j]))

    # Transformando em nao direcionado
    for i in range(300):
        for j in range(i, 300):
            _adj[i][j] = (abs(_adj[i][j]) + abs(_adj[j][i])) / 2
            _adj[j][i] = _adj[i][j]

    _G = nx.from_numpy_array(np.array(_adj))

    return _G

# #for alpha in range(5, 101, 5):
# #for p in range(1, 11):
# for intra in range(100, 5001, 300):
#     G = build_graph(f'output_intra_{intra}.txt')
#
#     C = nx.community.kernighan_lin_bisection(G, seed=123)
#
#     c = [0, 0]
#     for i in range(2):
#         c[i] = [k for k in C[i]]
#
#     m = [[0, 0], [0, 0]]
#     qt = [[0, 0], [0, 0]]
#
#     for (u, v, w) in G.edges(data=True):
#         gu = 0 if u in c[0] else 1
#         gv = 0 if v in c[0] else 1
#
#         m[gu][gv] += w['weight']
#         m[gv][gu] += w['weight']
#         qt[gu][gv] += 1
#         qt[gv][gu] += 1
#
#     for i in range(2):
#         for j in range(2):
#             m[i][j] /= qt[i][j]
#
#     print(intra, m[0][0], m[0][1], m[1][1])
#
#     continue
#
#
#     ok1, ok2 = 0, 0
#
#     for i in range(150):
#         if i in c[0]:
#             ok1 += 1
#         else:
#             ok2 += 1
#
#     for i in range(150, 300):
#         if i in c[1]:
#             ok1 += 1
#         else:
#             ok2 += 1
#
#     correct = max(ok1, ok2)
#
#     # print(c[0])
#     # print('')
#     # print(c[1])
#     # print('')
#     # print(correct)
#     # print('')
#     print(intra, str(round((correct / 300), 5)))

def kernig(path):
    G = build_graph(path)

    C = nx.community.kernighan_lin_bisection(G, seed=123)

    c = [0, 0]
    for i in range(2):
        c[i] = [k for k in C[i]]

    m = [[0, 0], [0, 0]]
    qt = [[0, 0], [0, 0]]

    for (u, v, w) in G.edges(data=True):
        gu = 0 if u in c[0] else 1
        gv = 0 if v in c[0] else 1

        m[gu][gv] += w['weight']
        m[gv][gu] += w['weight']
        qt[gu][gv] += 1
        qt[gv][gu] += 1

    for i in range(2):
        for j in range(2):
            m[i][j] /= qt[i][j]

    ok1, ok2 = 0, 0

    for i in range(150):
        if i in c[0]:
            ok1 += 1
        else:
            ok2 += 1

    for i in range(150, 300):
        if i in c[1]:
            ok1 += 1
        else:
            ok2 += 1

    print(c)
    print(ok1, ok2)


kernig(f'../out/giulio_guilherme/pearson_spike.txt')
exit(0)


G = build_graph(f'output_intra_4900.txt')

pesos = []

for e in G.edges.data():
    pesos.append(e[2]['weight'])

to_remove = []
mediana = sorted(pesos)[int(len(pesos)-600)]
for e in G.edges.data():
    if e[2]['weight'] < mediana:
        to_remove.append(e)

for e in to_remove:
    G.remove_edge(e[0], e[1])

#C = nx.community.louvain_communities(G, seed=123, resolution=0.9)
C = nx.community.girvan_newman(G)

#coms = algorithms.leiden(G)

print(tuple(sorted(c) for c in next(C)))