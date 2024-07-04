import math
import os
import sys

from matplotlib.widgets import TextBox

from calculator import *
from utils import *
from reader import *
import matplotlib.pyplot as plt
from datetime import datetime
import time
import scipy

def information_message():
    msg = ''
    msg += 'N = ' + params['N'] + ' | T = ' + params['T'] + '\n'
    msg += 'BURN = ' + str(int(round(float(params['BURN_T'])/1000))) + 'K\n'
    msg += 'p = ' + params['p'] + ' | q = ' + params['q'] + '\n'
    msg += 'mu_in = ' + params['mu_in'] + '\n'
    msg += '           ' + decimal_to_porc(params['intra_exchitatory_portion']) + ' excitatório\n'
    msg += 'mu_out = ' + params['mu_out'] + '\n'
    msg += '           ' + decimal_to_porc(params['inter_exchitatory_portion']) + ' excitatório\n'
    msg += 'mu = ' + decimal_to_porc(params['mu']) + '\n'

    msg += '\nSeed = ' + params['seed'] + '\n'
    msg += 'Samples = ' + params['samples'] + '\n'

    msg += f'\nThreshold: {threshold}' + '\n'
    msg += f'Precision: {precision}%' + '\n'
    msg += f'Recall: {recall}%' + '\n'

    msg += f'\nTrue Pos.: {tp}' + '\n'
    msg += f'True Neg.: {tn}' + '\n'
    msg += f'False Pos.: {fp}' + '\n'
    msg += f'False Neg.: {fn}' + '\n'

    return msg


last_time = start_time = time.time()

#input_folder_base = f'out/giulio_guilherme'
input_folder_base = f'out/teste'

if len(sys.argv) > 1:
    input_folder_base = sys.argv[1]

print(input_folder_base)

params = read_params(input_folder_base)

n = int(params['N'])
samples = int(params['samples'])
data = []
for i in range(n):
    data.append([])
    for j in range(n):
        data[i].append(0)

# for sample in range(samples):
#     input_folder = input_folder_base + '/' + str(sample + 1)
#     spikes = read_input('spike_trains', float, input_folder)
#     adj = read_input('adjacency_0_1', int, input_folder)
#
#     threshold = 0.25
#
#     sample_data = calculate_pearson_corr(spikes, n)
#     for i in range(n):
#         for j in range(n):
#             data[i][j] += sample_data[i][j] / samples

for sample in range(samples):
    input_folder = input_folder_base + '/' + str(sample + 1)
    spikes = read_input('spike_trains', float, input_folder)
    adj = read_input('adjacency_0_1', int, input_folder)

    threshold = 0.25

    sample_data = calculate_pearson_corr(spikes, n)
    for i in range(n):
        for j in range(n):
            data[i][j] += sample_data[i][j] / samples

with open(f'../{input_folder}/pearson.txt', 'w') as file:
    for i in range(n):
        for j in range(n):
            file.write(str(data[i][j]) + ' ')
        file.write('\n')

exit(0)

# Colocando valores entre 0 e 1
# data_min, data_max = 1, 0
# for i in range(n):
#     for j in range(n):
#         data_min = min(data_min, data[i][j])
#         data_max = max(data_max, data[i][j])
#
# for i in range(n):
#     for j in  range(n):
#         data[i][j] = (data[i][j] - data_min)/(data_max - data_min)


with open(f'../{input_folder_base}/pearson.txt', 'w') as file:
    for i in range(n):
        for j in range(n):
            file.write(str(data[i][j]) + ' ')
        file.write('\n')


intra, inter = calculate_groups_averages(data, n)

plt.text(0, 1.2 * n, 'Média intra: ' + str(round(intra[0], 5)) + ' / ' + str(round(intra[1], 5)) +
         '\nMédia inter: ' + str(round(inter[0], 5)) + ' / ' + str(round(inter[1], 5)))

precision, recall, tp, fp, fn, tn = calculate_prediction_quality(data, adj, threshold)


plt_text = plt.text(1.03 * n, 1.05 * n, information_message())

plt.imshow(data, cmap='seismic', vmin=-1, vmax=1)
#plt.title(f'EWMA - Alpha = {params["alpha_ewma"]}')
plt.title(f'Spike Trains - Delay de 1 u.t.')
plt.tight_layout()


def submit(text):
    global threshold, precision, recall, tp, fp, fn, tn

    threshold = float(text)
    precision, recall, tp, fp, fn, tn = calculate_prediction_quality(data, adj, threshold)

    plt_text.set_text(information_message())
    plt.draw()


axbox = plt.axes([0.828, 0.05, 0.08, 0.075])
text_box = TextBox(axbox, 'Threshold: ', initial=str(threshold))
text_box.on_submit(submit)

plt.show()
