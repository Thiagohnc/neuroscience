import sys

import numpy as np
from sklearn.cluster import SpectralClustering

from reader import read_pearson

obligatory_args = ['input_folder', 'n_clusters', 'pot', 'seed', 'filename']
args = dict()
for arg in sys.argv[1:]:
    name, val = arg.split('=')[0], arg.split('=')[1]
    args[name] = val

for o in obligatory_args:
    if o not in args.keys():
        raise Exception(o + ' must be an argument')

sim = read_pearson(args['input_folder'])

for i in range(len(sim)):
    for j in range(i + 1):
        sim[i][j] = sim[j][i] = (abs(sim[i][j]) + abs(sim[j][i]) / 2) ** int(args['pot'])

clustering = SpectralClustering(n_clusters=int(args['n_clusters']),
                                assign_labels='cluster_qr',
                                affinity='precomputed',
                                random_state=int(args['seed'])).fit(np.array(sim, dtype=np.float64))

with open(args['input_folder'] + '/' + args['filename'], 'w') as file:
    for i in range(len(sim)):
        file.write(str(clustering.labels_[i]) + '\n')
