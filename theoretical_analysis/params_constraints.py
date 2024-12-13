# Functions to calculate constraints on model parameters

# Imports
import numpy as np

def first_constraint(N, alpha_1, mu, beta, p, q, gamma):
    """
    Compute the maximum value that mu_{IN} can attains
    without violating the first constraint.
    
    Parameters
    ----------
    N : int
        Number of neurons in the network.
    alpha_1 : float
        Fraction of neurons in block 1.
    mu : float
        Firing baseline, which represents the base activity level of neurons.
    beta : float
        Proportion of excitatory edges in the network, where 0 <= beta <= 1.
    p : float
        Probability of intra-block edges, i.e., the likelihood that an edge exists between two neurons within the same block.
    q : float
        Probability of inter-block edges, i.e., the likelihood that an edge exists between two neurons in different blocks.
    gamma : float
        Ratio of mu_{IN} to mu_{OUT}, i.e., gamma = mu_{IN}/mu_{OUT}.
    
    Returns
    ----------
    float
        The maximum value of mu_{IN} that can be attained without violating the first constraint.
    """

    # Compute fraction of neurons in block 2
    alpha_2 = 1 - alpha_1
    
    # Compute maximum between v_1 and v_2
    v_1 = p*alpha_1 + (q*alpha_2/gamma)
    v_2 = p*alpha_2 + (q*alpha_1/gamma)
    v = max(v_1, v_2)

    # Compute the denominator of the first constraint
    denom = np.sqrt(2*np.log(2*N)/N) + (2*beta - 1)*v

    # Compute the maximum value that mu_{IN} can attains
    mu_IN = (1 - mu)/denom
    return mu_IN


def second_constraint(N, mu):
    """
    Compute the maximum value that mu_{IN} can attains
    without violating the second constraint.
    
    Parameters
    ----------
    N : int
        Number of neurons in the network.
    mu : float
        Firing baseline, which represents the base activity level of neurons.
    
    Returns
    ----------
    float
        The maximum value of mu_{IN} that can be attained without violating the second constraint.
    """

    # Compute the denominator of the second constraint
    denom = np.sqrt(2*np.log(2*N)/N)

    # Compute the maximum value that mu_{IN} can attains
    mu_IN = mu/denom
    return mu_IN


def constraints(N, alpha_1, mu, beta, p, q, gamma):
    """
    Compute the maximum value that mu_{IN} can attains
    without violating neither of the constraints.

    This function uses `first_constraint` and `second_constraint` to compute the maximum value for
    mu_{IN} in both scenarios and returns the minumum value between them.

    Parameters
    ----------
    N : int
        Number of neurons in the network.
    alpha_1 : float
        Fraction of neurons in block 1.
    mu : float
        Firing baseline, which represents the base activity level of neurons.
    beta : float
        Proportion of excitatory edges in the network, where 0 <= `beta` <= 1.
    p : float
        Probability of intra-block edges, i.e., the likelihood that an edge exists between two neurons within the same block.
    q : float
        Probability of inter-block edges, i.e., the likelihood that an edge exists between two neurons in different blocks.
    gamma : float
        Ratio of mu_{IN} to mu_{OUT}, i.e., gamma = mu_{IN}/mu_{OUT}.

    Returns
    ----------
    float
        The maximum value of mu_{IN} that can be attained without violating neither of the constraints.
    """

    # Compute the maximum value mu_{IN} can attains without violating the first constraint
    c_1 = first_constraint(N, alpha_1, mu, beta, p, q, gamma)

    # Compute the maximum value mu_{IN} can attains without violating the second constraint
    c_2 = second_constraint(N, mu)

    # Compute the minimum value between c_1 and c_2
    return min(c_1, c_2)