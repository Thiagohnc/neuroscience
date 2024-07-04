from matplotlib import pyplot as plt

from reader import read_params
from utils import decimal_to_porc, format_int
from mds import *

input_folder = f'out/teste'
params = read_params(input_folder)


def information_message():
    msg = ''
    msg += f'N = {params['N']} | T = {format_int(params['T'])} ({format_int(params['BURN_T'])} Burned)\n'
    msg += 'N = ' + params['N'] + '\n'
    msg += 'p = ' + params['p'] + ' | q = ' + params['q'] + '\n'
    msg += r'$\mu_{\mathrm{in}} = ' + params['mu_in'] + '$ '
    msg += '(' + decimal_to_porc(params['intra_exchitatory_portion']) + ' excitatório)\n'
    msg += r'$\mu_{\mathrm{out}} = ' + params['mu_out'] + '$ '
    msg += '(' + decimal_to_porc(params['inter_exchitatory_portion']) + ' excitatório)\n'
    msg += r'$\mu = ' + params['mu'] + '$\n'
    msg += '\n\nSeed = ' + params['seed'] + '\n'

    return msg

def plot_acertos_por_t():
    n_plots = 10
    labels = ['pot ' + str(i) for i in range(1,11)]
    xlabel = 'T'
    ylabel = '% Acertos'
    title = 'Spectral Clustering\nEvolução de acertos conforme T aumenta'

    x = []
    y = [[] for _ in range(n_plots)]

    with open('input_medias.txt') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip().split(' ')
            x.append(float(line[0]))
            for i in range(n_plots):
                y[i].append(float(line[i + 1]))

    for i in range(n_plots):
        if not labels:
            plt.plot(x, y[i])
        else:
            plt.plot(x, y[i], label=labels[i])

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)

    plt.title(title)

    print(y)

    plt.text(1.1 * max(x), min([min(yy) for yy in y]), information_message())

    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.legend()
    plt.show()


def plot_barra_pot(input_folder, pot_max):
    xlabel = 'Potência'
    ylabel = '% de Acertos'
    title = 'Spectral clustering\n Porcentagem de acertos para cada potência testada'

    x = [i for i in range(1, pot_max + 1)]
    y = [calc_spectral(input_folder, pot=p) for p in x]
    plt.bar(x, y)

    plt.text(1.1 * max(x), 0.5*min(y), information_message())

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)

    plt.tight_layout()
    plt.show()

plot_barra_pot(f'../out/teste/1/pearson.txt', 20)