import math

import numpy as np
import matplotlib.pyplot as plt
import scipy

spikes = []

with open('out', 'r') as file:
    lines = file.readlines()
    for line in lines:
        line = line.strip().split(' ')
        spikes.append([])
        for spk in line:
            spikes[len(spikes) - 1].append(float(spk))

data = []
n = 300

aa, aaqt = 0, 0
ab, abqt = 0, 0
ba, baqt = 0, 0
bb, bbqt = 0, 0

for i in range(n):
    data.append([])
    for j in range(n):
        if j == 0:
            print(i)
        corr = scipy.stats.pearsonr(spikes[i], spikes[j]).statistic
        data[i].append(0 if math.isnan(corr) else corr)
        #data[i].append(spikes[i][j])

        if i != j and not math.isnan(corr):
            if i < j < n/2:
                aa += corr
                aaqt += 1
            if i < n/2 <= j:
                ab += corr
                abqt += 1
            if i >= n/2 > j:
                ba += corr
                baqt += 1
            if j >= i >= n/2:
                bb += corr
                bbqt += 1

#print(aa/aaqt, ab/abqt, ba/baqt, bb/bbqt)
#print(aa, aaqt, ab, abqt, ba, baqt, bb, bbqt)

#print('Média intra:', (aa + bb)/(aaqt + bbqt), '|', 'Média inter:', (ab + ba)/(abqt + baqt))

plt.text(90, n + 50, 'Média intra: ' + str(round((aa + bb)/(aaqt + bbqt), 5)) + '\nMédia inter: ' + str(round((ab + ba)/(abqt + baqt),5)))

plt.imshow(data)
plt.title('intra = N(0.15,0.3) / inter = N(0.05,0.1)')
plt.tight_layout()
plt.show()