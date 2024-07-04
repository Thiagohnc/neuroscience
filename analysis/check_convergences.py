from matplotlib import pyplot as plt
from utils import *

from reader import *

def information_message():
    msg = ''
    msg += 'N = ' + params['N'] + ' | T = ' + params['T'] + '\n'
    msg += 'p = ' + params['p'] + ' | q = ' + params['q'] + '\n'
    msg += 'q = ' + params['q'] + '\n'
    msg += 'Intra $\sim \mathcal{N}$(' + params['w_intra_mean'] + ', ' + params['w_intra_std'] + ')\n'
    msg += '           ' + decimal_to_porc(params['intra_exchitatory_portion']) + ' excitatório\n'
    msg += 'Inter $\sim \mathcal{N}$(' + params['w_inter_mean'] + ', ' + params['w_inter_std'] + ')\n'
    msg += '           ' + decimal_to_porc(params['inter_exchitatory_portion']) + ' excitatório\n'
    msg += 'Ativ. Espontânea = ' + decimal_to_porc(params['auto_activ']) + '\n'
    msg += '\n\nSeed = ' + params['seed'] + '\n'
    msg += '\nSamples = ' + params['samples'] + '\n'

    return msg

intra = 5000

input_folder_base = f'out/change_intra/{intra}'
params = read_params(input_folder_base)

pi = [0] * int(params['T'])
pi_sample = []

for sample in range(int(params['samples'])):
    print(sample)
    input_folder = input_folder_base + '/' + str(sample + 1)
    spikes = read_input('spike_trains', float, input_folder)
    adj = read_input('adjacency_0_1', int, input_folder)

    mean = [[0] * int(params['T'])] * int(params['N'])
    error = [[0] * int(params['T'])] * int(params['N'])

    for v in range(int(params['N'])):
        ac = 0
        for t in range(int(params['T'])):
            ac += spikes[v][t]
            mean[v][t] = ac/(t+1)

    for v in range(int(params['N'])):
        for t in range(int(params['T'])):
            error[v][t] = abs(mean[v][t] - mean[v][-1])

    pi_sample.append([0] * int(params['T']))

    for t in range(int(params['T'])):
        for v in range(int(params['N'])):
            pi_sample[-1][t] += error[v][t]
        pi_sample[-1][t] /= int(params['N'])
        pi[t] += pi_sample[-1][t] / int(params['samples'])

x = [i + 1 for i in range(int(params['T']))]

plt.plot(x, pi)

plt.xlabel('t')
plt.ylabel('Média da diferença')

plt.title('Média da diferença entre a média atual\n e a média final (spike trains)')

plt.text(1.1 * max(x), min(pi), information_message())

plt.tight_layout()
#plt.legend()
plt.show()