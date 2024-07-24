from sklearn.manifold import MDS
from sklearn.cluster import KMeans, SpectralClustering
from matplotlib import pyplot as plt
import numpy as np
from math import *
from reader import *

#a, b = 1, 0
#for i in range(len(D)):
#   for j in range(len(D[i])):
#       D[i][j] = log(D[i][j] / (1 - D[i][j]))
#       a = min(a, D[i][j])
#       b = max(b, D[i][j])

#print(a, b)

# fig = plt.figure()
# ax = fig.add_subplot(projection='3d')

def calc_kmeans():
    d = read_pearson('../out/teste/pearson.txt')

    for i in range(len(d)):
        for j in range(i + 1):
            d[i][j] = d[j][i] = (0.99 - (abs(d[i][j]) + abs(d[i][j])) / 2) ** 40

    for i in range(len(d)):
      for j in range(len(d[i])):
          d[i][j] = log(d[i][j] / (1 - d[i][j]))

    model = MDS(n_components=2, dissimilarity='precomputed', random_state=12345)
    out = model.fit_transform(d)
    # ax.scatter(out[:, 0], out[:, 1], out[:, 2], c=['red']*150+['blue']*150)
    plt.scatter(out[:, 0], out[:, 1], c=['red']*150+['blue']*150)
    # plt.scatter(out[:, 0], [0] * len(out[:, 0]), c=['red']*150+['blue']*150)
    # ax.axis('equal')
    plt.axis('equal')
    plt.show()

    kmeans = KMeans(n_clusters=2, random_state=12345)
    kmeans.fit(out)

    l = kmeans.labels_

    print(kmeans.labels_)

    ok = 0
    nok = 0
    for i in range(300):
        for j in range(300):
            if i != j:
                if (i < 150 and j < 150) or (i >= 150 and j >= 150):
                    if l[i] == l[j]:
                        ok += 1
                    else:
                        nok += 1
                else:
                    if l[i] == l[j]:
                        nok += 1
                    else:
                        ok += 1

    print(ok, nok, ok/(ok+nok), nok/(ok+nok))

def calc_spectral(input_folder, verbose=False, pot=2):
    d = read_pearson(input_folder)

    for i in range(len(d)):
        for j in range(i + 1):
            d[i][j] = d[j][i] = (abs(d[i][j] + abs(d[i][j])) / 2) ** pot

    clustering = SpectralClustering(n_clusters=2,
            #assign_labels='cluster_qr',
            #assign_labels='kmeans',
            assign_labels='discretize',
            affinity='precomputed',
            random_state=12345).fit(np.array(d))

    if verbose:
        print('Spectral Clustering:')
        print(f'D[i][j] = D[j][i] = (abs(D[i][j] + abs(D[i][j])) / 2) ** {pot}')

    l = clustering.labels_

    if verbose:
        print('Labels:', clustering.labels_)

    ok = 0
    nok = 0
    for i in range(300):
        for j in range(300):
            if i != j:
                if (i < 150 and j < 150) or (i >= 150 and j >= 150):
                    if l[i] == l[j]:
                        ok += 1
                    else:
                        nok += 1
                else:
                    if l[i] == l[j]:
                        nok += 1
                    else:
                        ok += 1
    if verbose:
        print('Ok:', ok,'/ Nok:', nok, '/ % Ok:', round(ok / (ok + nok), 2), '/ % Nok:', round(nok / (ok + nok), 2))
    return ok / (ok + nok)

if __name__ == '__main__':

    # for i in range(10000, 320001, 10000):
    #     a = [str(calc_spectral(f'../out/change_t/{i}/1/pearson.txt', pot=p)) for p in range(1, 11)]
    #     print(i, " ".join(a))
    # calc_kmeans()

    x = [i for i in range(1, 21)]
    a = [calc_spectral(f'../out/teste/1/pearson.txt', pot=p) for p in x]
    plt.bar(x, a)
    plt.show()