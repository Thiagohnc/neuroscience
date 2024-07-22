import os

from matplotlib import pyplot as plt

from reader import read_params
from utils import decimal_to_porc, format_int
from mds import *

input_folder = f'../out/change_muin_greaterpq_10samples'
params = read_params(input_folder + '/1.0/')


def information_message():
    msg = ''
    msg += f'N = {params['N']} | T = {format_int(params['T'])} ({format_int(params['BURN_T'])} Burned)\n'
    #msg += f'N = {params['N']} | T =  ({format_int(params['BURN_T'])} Burned)\n'
    msg += 'N = ' + params['N'] + '\n'
    msg += 'p = ' + params['p'] + ' | q = ' + params['q'] + '\n'
    msg += r'$\mu_{\mathrm{in}} = 1.0 - 5.0$ '
    #msg += r'$\mu_{\mathrm{in}} = ' + params['mu_in'] + '$ '
    msg += '(' + decimal_to_porc(params['intra_exchitatory_portion']) + ' excitatório)\n'
    msg += r'$\mu_{\mathrm{out}} = ' + params['mu_out'] + '$ '
    msg += '(' + decimal_to_porc(params['inter_exchitatory_portion']) + ' excitatório)\n'
    msg += r'$\mu = ' + params['mu'] + '$\n'
    msg += '\n\nSamples = ' + params['samples'] + '\n'
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
    yl = []
    samples = int(params['samples'])
    for sample in range(samples):
        yl.append([calc_spectral(f'{input_folder}/{sample+1}/pearson', pot=p) for p in x])
    y = [sum([yl[i][j] for i in range(len(yl))])/samples for j in range(len(x))]

    plt.bar(x, y)
    plt.text(1.1 * max(x), 0.3*min(y), information_message())

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)

    plt.tight_layout()
    plt.show()


def plot_by_t(input_folder, K):
    xlabel = 'T (log10)'
    ylabel = '% de Acertos'
    title = 'Spectral clustering\n Porcentagem de acertos por tempo'

    x = [i for i in range(3, 6 + 1)]
    y = []
    samples = int(params['samples'])
    for k in K:
        yl = []
        for sample in range(samples):
            print([f'../out/k{k}_t{p}/{sample + 1}/pearson' for p in x])
            yl.append([calc_spectral(f'../out/k{k}_t{p}/{sample + 1}/pearson', pot=2) for p in x])
            print(yl)
        print(samples)
        y.append([sum([yl[i][j] for i in range(len(yl))]) / samples for j in range(len(x))])

    for i in range(len(K)):
        plt.plot(x, y[i], label=f'k = {i}')
    plt.text(1.1 * max(x), 0.6, information_message())

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)

    plt.legend()

    plt.tight_layout()
    plt.show()

def plot_in_by_out(input_folder):
    xlabel = r'$\mu_{\mathrm{in}}$'
    ylabel = '% de Acertos'
    title = 'Spectral clustering\n ' + r'Porcentagem de acertos por $\mu_{\mathrm{in}} (Pot = 1)$'

    x, y = [], []
    samples = int(params['samples'])
    for dir in sorted(os.listdir(input_folder)):
        print(dir)
        try:
            yl = []
            for sample in range(samples):
                yl.append(calc_spectral(f'{input_folder}/{dir}/{sample + 1}/pearson', pot=1))
            x.append(float(dir))
            y.append(sum([i for i in yl]) / samples)
            print(y)
        except Exception as e:
            print(e)
            continue

    plt.plot(x, y)
    plt.text(1.1 * max(x), 0.6, information_message())

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)

    plt.yticks([i/10 for i in range(5, 10 + 1)])

    plt.tight_layout()
    plt.show()


#plot_barra_pot(input_folder, 10)
plot_in_by_out(input_folder)
