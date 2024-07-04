from reader import *

input_folder = f'out/change_intra/{2000}'

spikes = read_input('spike_trains', float, input_folder)

for j in range(len(spikes[0])):
    print(sum(spikes[:][j]))