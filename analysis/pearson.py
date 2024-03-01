import math

from matplotlib.widgets import TextBox

from utils import *
from reader import *
import matplotlib.pyplot as plt
import scipy

params = read_params()
spikes = read_input('ewma', float)
adj = read_input('adjacency_0_1', int)

message = ''
message += 'N = ' + params['N'] + ' | T = ' + params['T'] + '\n'
message += 'p = ' + params['p'] + ' | q = ' + params['q'] + '\n'
message += 'Intra $\sim \mathcal{N}$(' + params['w_intra_mean'] + ', ' + params['w_intra_std'] + ')\n'
message += '           ' + decimal_to_porc(params['intra_exchitatory_portion']) + ' excitatório\n'
message += 'Inter $\sim \mathcal{N}$(' + params['w_inter_mean'] + ', ' + params['w_inter_std'] + ')\n'
message += '           ' + decimal_to_porc(params['inter_exchitatory_portion']) + ' excitatório\n'
message += 'Ativ. Espontânea = ' + decimal_to_porc(params['auto_activ']) + '\n'
message += '\n\nSeed = ' + params['seed'] + '\n'

n = int(params['N'])

aa, aaqt = 0, 0
ab, abqt = 0, 0
ba, baqt = 0, 0
bb, bbqt = 0, 0

threshold = 0.5
tp, fp, fn, tn = 0, 0, 0, 0

data = []
for i in range(n):
    data.append([])
    for j in range(n):
        if j == 0:
            print(i)
        corr = 1#scipy.stats.pearsonr(spikes[i][:-1], spikes[j][1:]).statistic
        data[i].append(0 if math.isnan(corr) else corr)
        # data[i].append(spikes[i][j])

        if i != j:
            if adj[i][j] == 1 and abs(data[i][j]) >= threshold:
                tp += 1
            if adj[i][j] == 0 and abs(data[i][j]) >= threshold:
                fp += 1
            if adj[i][j] == 1 and abs(data[i][j]) < threshold:
                fn += 1
            if adj[i][j] == 0 and abs(data[i][j]) < threshold:
                tn += 1

        if i != j and not math.isnan(corr):
            if i < j < n / 2:
                aa += abs(corr)
                aaqt += 1
            if i < n / 2 <= j:
                ab += abs(corr)
                abqt += 1
            if i >= n / 2 > j:
                ba += abs(corr)
                baqt += 1
            if j >= i >= n / 2:
                bb += abs(corr)
                bbqt += 1

plt.text(0, 1.2 * n, 'Média intra: ' + str(round(aa / aaqt, 5)) + ' / ' + str(round(bb / bbqt, 5)) +
         '\nMédia inter: ' + str(round(ab / abqt, 5)) + ' / ' + str(round(ba / baqt, 5)))

precision = float('nan') if tp + fp == 0 else round(100*tp/(tp + fp), 1)
recall = float('nan') if tp + fn == 0 else round(100*tp/(tp + fn), 1)

message += f'\n\nThreshold: {threshold}' + '\n'
message += f'Precision: {precision}%' + '\n'
message += f'Recall: {recall}%' + '\n'

message += f'\n\nTrue Pos.: {tp}' + '\n'
message += f'True Neg.: {tn}' + '\n'
message += f'False Pos.: {fp}' + '\n'
message += f'False Neg.: {fn}' + '\n'

plt_text = plt.text(1.03 * n, 1.05 * n, message)

plt.imshow(data, cmap='seismic', vmin=-1, vmax=1)
plt.title(f'EWMA - Alpha = {params["alpha_ewma"]}')
plt.tight_layout()

def submit(text):
    global message, threshold

    threshold = float(text)
    message = re.sub(r'(Threshold:)[^\n]*', fr'\1 {text}', message)
    plt_text.set_text(message)
    plt.draw()

axbox = plt.axes([0.828, 0.05, 0.08, 0.075])
text_box = TextBox(axbox, 'Threshold: ', initial='0.5')
text_box.on_submit(submit)



plt.show()
