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
