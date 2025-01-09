# Functions to calculate constraints on model parameters

# Imports
import numpy as np

def constraints(lambda_base, p, q, alpha_1, gamma):
    """
    Compute the strict upper bound for mu_{~}.
    
    Parameters
    ----------
    alpha_1 : float
        Fraction of neurons in block 1.
    lambda_base : float
        Firing baseline, which represents the base activity level of neurons.
    p : float
        Probability of intra-block edges, i.e., the likelihood that an edge exists between two neurons within the same block.
    q : float
        Probability of inter-block edges, i.e., the likelihood that an edge exists between two neurons in different blocks.
    gamma : float
        Ratio of mu_{~} to mu_{|~}, i.e., gamma = mu_{~}/mu_{|~}.

    Returns
    ----------
    float
        The strict upper bound for mu_{~}.
    """

    # Compute fraction of neurons in block 2
    alpha_2 = 1 - alpha_1

    c1 = alpha_1*p + (alpha_2*q/gamma)
    c2 = alpha_2*p + (alpha_1*q/gamma)

    M = max(c1, c2)

    r1 = lambda_base/M

    print("Restrição 1:", r1)

    r2 = (1 - lambda_base)/M

    print("Restrição 2:", r2)

    return min(r1, r2)