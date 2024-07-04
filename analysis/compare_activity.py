from matplotlib import pyplot as plt

from reader import *

input_folder_base = f'out/daniel'
params = read_params(input_folder_base)

n = int(params['N'])
T = int(params['T'])

spikes = read_input('spike_trains', float, f'{input_folder_base}/1')

x = [i for i in range(1, T + 1)]
y1 = [0] * T
y2 = [0] * T

for i in range(len(spikes)):
    for j in range(len(spikes[i])):
        if i < n//2:
            y1[j] += spikes[i][j]
        else:
            y2[j] += spikes[i][j]

limit=9900

plt.plot(x[limit:], y1[limit:], label='Grupo A')
plt.plot(x[limit:], y2[limit:], label='Grupo B')

plt.legend()
plt.show()