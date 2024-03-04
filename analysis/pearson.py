import math

from matplotlib.widgets import TextBox

from calculator import *
from utils import *
from reader import *
import matplotlib.pyplot as plt
import scipy


def information_message():
    msg = ''
    msg += 'N = ' + params['N'] + ' | T = ' + params['T'] + '\n'
    msg += 'p = ' + params['p'] + ' | q = ' + params['q'] + '\n'
    msg += 'Intra $\sim \mathcal{N}$(' + params['w_intra_mean'] + ', ' + params['w_intra_std'] + ')\n'
    msg += '           ' + decimal_to_porc(params['intra_exchitatory_portion']) + ' excitatório\n'
    msg += 'Inter $\sim \mathcal{N}$(' + params['w_inter_mean'] + ', ' + params['w_inter_std'] + ')\n'
    msg += '           ' + decimal_to_porc(params['inter_exchitatory_portion']) + ' excitatório\n'
    msg += 'Ativ. Espontânea = ' + decimal_to_porc(params['auto_activ']) + '\n'
    msg += '\n\nSeed = ' + params['seed'] + '\n'

    msg += f'\n\nThreshold: {threshold}' + '\n'
    msg += f'Precision: {precision}%' + '\n'
    msg += f'Recall: {recall}%' + '\n'

    msg += f'\n\nTrue Pos.: {tp}' + '\n'
    msg += f'True Neg.: {tn}' + '\n'
    msg += f'False Pos.: {fp}' + '\n'
    msg += f'False Neg.: {fn}' + '\n'

    return msg


params = read_params()
spikes = read_input('ewma', float)
adj = read_input('adjacency_0_1', int)

n = int(params['N'])

threshold = 0.5

data = calculate_pearson_corr(spikes, n, verbose=True)

intra, inter = calculate_groups_averages(data, n)

plt.text(0, 1.2 * n, 'Média intra: ' + str(round(intra[0], 5)) + ' / ' + str(round(intra[1], 5)) +
         '\nMédia inter: ' + str(round(inter[0], 5)) + ' / ' + str(round(inter[1], 5)))

precision, recall, tp, fp, fn, tn = calculate_prediction_quality(data, adj, threshold)


plt_text = plt.text(1.03 * n, 1.05 * n, information_message())

plt.imshow(data, cmap='seismic', vmin=-1, vmax=1)
plt.title(f'EWMA - Alpha = {params["alpha_ewma"]}')
plt.tight_layout()


def submit(text):
    global threshold, precision, recall, tp, fp, fn, tn

    threshold = float(text)
    precision, recall, tp, fp, fn, tn = calculate_prediction_quality(data, adj, threshold)

    plt_text.set_text(information_message())
    plt.draw()


axbox = plt.axes([0.828, 0.05, 0.08, 0.075])
text_box = TextBox(axbox, 'Threshold: ', initial='0.5')
text_box.on_submit(submit)

plt.show()
