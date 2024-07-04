import math
import threading

from tqdm import tqdm

from scipy import stats

from reader import read_input


def calculate_prediction_quality(data, truth, threshold):
    tp, fp, fn, tn = 0, 0, 0, 0

    for i in range(len(data)):
        for j in range(len(data[i])):
            if i == j:
                continue

            if truth[i][j] == 1 and abs(data[i][j]) >= threshold:
                tp += 1
            if truth[i][j] == 0 and abs(data[i][j]) >= threshold:
                fp += 1
            if truth[i][j] == 1 and abs(data[i][j]) < threshold:
                fn += 1
            if truth[i][j] == 0 and abs(data[i][j]) < threshold:
                tn += 1

    precision = float('nan') if tp + fp == 0 else round(100 * tp / (tp + fp), 1)
    recall = float('nan') if tp + fn == 0 else round(100 * tp / (tp + fn), 1)

    return precision, recall, tp, fp, fn, tn


def calculate_groups_averages(data, n):
    intra, inter = [0, 0], [0, 0]

    for i in range(n):
        for j in range(n):
            if i != j:
                if i < n // 2 and j < n // 2:
                    intra[0] += abs(data[i][j])
                if i < n // 2 <= j:
                    inter[0] += abs(data[i][j])
                if i >= n // 2 > j:
                    inter[1] += abs(data[i][j])
                if i >= n // 2 and j >= n // 2:
                    intra[1] += abs(data[i][j])

    inter[0] /= (n // 2) * ((n + 1) // 2)
    inter[1] /= (n // 2) * ((n + 1) // 2)
    intra[0] /= (n // 2) ** 2 - (n // 2)
    intra[1] /= ((n + 1) // 2) ** 2 - ((n + 1) // 2)

    return intra, inter


def calculate_pearson_corr(spikes, n):
    data = []
    for i in tqdm(range(n)):
        data.append([0] * n)
        for j in range(n):
            corr = stats.pearsonr(spikes[i][:-1], spikes[j][1:]).statistic
            data[i][j] = 0 if math.isnan(corr) else corr
    return data


# def calculate_pearson_corr(spikes, n):
#     data = []
#     for i in tqdm(range(n)):
#         data.append([])
#         for j in range(n):
#             corr = stats.pearsonr(spikes[i][:-1], spikes[j][1:]).statistic
#             data[i].append(0 if math.isnan(corr) else corr)
#     return data


def calculate_precision_recall_over_time(tmin, tmax, step, n, adj, threshold=0.5):
    for t in range(tmin, tmax, step):
        spikes = read_input('ewma', float, tmax=t)
        data = calculate_pearson_corr(spikes, n)
        precision, recall, _, _, _, _ = calculate_prediction_quality(data, adj, threshold)
        print(t, precision, recall)
