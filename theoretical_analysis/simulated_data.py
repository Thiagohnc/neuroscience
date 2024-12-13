# Functions to obtain simulated data

# Imports
import numpy as np
import pandas as pd

def get_adj(path):
    adj = pd.read_csv(path+'\\adjacency_0_1', sep=' ', header=None)
    adj = adj.drop(adj.columns[-1], axis=1) #remove NaN's column
    return adj

def get_weight(path):
    eta = pd.read_csv(path+'\\adjacency_weights', sep=' ', header=None)
    eta = eta.drop(eta.columns[-1], axis=1)
    eta = eta/eta.shape[0]
    return eta

def get_avg(path):
    avg = pd.read_csv(path+'\\spike_average', sep=' ', header=None)
    return avg

def get_var(path):
    var = pd.read_csv(path+'\\spike_variance', sep=' ', header=None)
    return var

def get_corr(path):
    corr = pd.read_csv(path+'\\pearson', sep=' ', header=None)
    corr = corr.drop(corr.columns[-1], axis=1)
    return corr

def std_matrix(var):
    stdev = var[0].apply(np.sqrt)
    return pd.DataFrame(np.outer(stdev, stdev))