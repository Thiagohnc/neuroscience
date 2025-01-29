# Functions concerning firing's theoretical properties

#Imports
import numpy as np

def theoretical_mean(A, mu):
    """
    Compute the theoretical firing mean for each neuron.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N).
    mu : float
        Firing baseline, which represents the base activity level of neurons.

    Returns
    ----------
    numpy.ndarray
        The vector of theoretical firing mean for the neurons.
    """
    
    # Get the number of neurons from the `A` matrix
    N = A.shape[0]

    # Create an identity matrix of size N x N
    I = np.eye(N)

    # Create a vector of ones of size N
    vector_1s = np.ones(N)

    # Compute the vector of firing mean for each neuron
    m = np.linalg.solve((I - A.T), mu*vector_1s)
    m = np.asarray(m)

    return m


def theoretical_variance(A, mu):
    """
    Compute the theoretical firing variance for each neuron.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N).
    mu : float
        Firing baseline, which represents the base activity level of neurons.

    Returns
    ----------
    numpy.ndarray
        The vector of theoretical firing variance for the neurons.
    """
    
    # Get the number of neurons from the `A` matrix
    N = A.shape[0]

    # Create an identity matrix of size N x N
    I = np.eye(N)

    # Create a vector of ones of size N
    vector_1s = np.ones(N)

    # Compute the vector of firing mean for each neuron
    m = np.linalg.solve((I - A.T), mu*vector_1s)
    m = np.asarray(m)

    # Compute the vector of firing variance for each neuron
    v = (1 - m)*m

    return v


def create_L_matrix(A):
    """
    Create a matrix L by computing the Kronecker product of the transpose
    of matrix A with itself, and then set specific rows to zero.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N). This matrix is used to compute the Kronecker product.

    Returns
    ----------
    numpy.ndarray
        A square matrix L of shape (N^2, N^2), where certain rows are set to zero.
    """

    # Get the size of the matrix A
    N = A.shape[0]

    # Compute the Kronecker product of the transpose of A with itself
    L = np.kron(A.T, A.T)

    # List of rows to set to zero
    zero_rows = []
    for i in range(N):
        # Calculate the row index to be set to zero
        x = (i+1)*(N+1) - N - 1
        zero_rows.append(x)

    # Set the identified rows to zero
    L[zero_rows, :] = 0

    # Return the modified matrix L
    return L


def theoretical_covariance(A, v, delay_1 = False):
    """
    Solve the matrix equation for the vectorized Sigma,
    and transform it back into a (N, N) matrix.
    Compute the covariance matrix.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N) used to compute the matrix L through the `create_L_matrix` function
    v : numpy.ndarray
        A 2D array of shape (1, N) representing the variance of firings from each neuron.
    delay_1 : boolean
        Whether we are looking for a delay of 1 or not.

    Returns
    ----------
    numpy.ndarray
        A square matrix of shape (N, N) representing the covariance matrix between the neurons firing.
    """
    # Create matrix L using the function `create_L_matrix`
    L = create_L_matrix(A)

    # Get the size of matrix A
    N = A.shape[0]

    # Form the diagonal matrix DV using the variance vector
    diag_v = np.diag(v)

    # Flatten the diagonal matrix into a vector
    flat_diag_v = diag_v.flatten('F')

    # Create the identity matrix of size N^2 x N^2
    I = np.eye(N**2)

    # Compute the solution to the matrix equation
    sigma = np.matmul(np.linalg.inv(I - L), flat_diag_v)

    # Reshape the result back to a (N, N) matrix and return the transpose (`reshape` function will reshape the matrix row-wise instead of column-wise)
    sigma = sigma.reshape(N,N).T

    # For delay 1 covariance
    if delay_1:
        sigma_1 = np.matmul(A, sigma)
        return sigma_1
    else:
        return sigma

def theoretical_correlation(A, v, delay_1=False):
    """
    Solve the matrix equation for the vectorized Sigma,
    and transform it back into a (N, N) matrix.
    Compute the correlation matrix.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N) used to compute the matrix L through the `create_L_matrix` function
    v : numpy.ndarray
        A 2D array of shape (1, N) representing the variance of firings from each neuron.
    delay_1 : boolean
        Whether we are looking for a delay of 1 or not.

    Returns
    ----------
    numpy.ndarray
        A square matrix of shape (N, N) representing the correlation matrix between the neurons firing.
    """
    sigma = theoretical_covariance(A, v, delay_1)

    # Compute the standard deviation from variance
    stdev = np.sqrt(v)
    # Create the outer product of standard deviation
    stdev_matrix = np.outer(stdev, stdev)

    return sigma/stdev_matrix
    

def asymptotic_mean(lambda_base, p, q, beta, alpha_1, mu_IN, mu_OUT):

    alpha_2 = 1 - alpha_1

    g11 = (2*beta - 1)*p*mu_IN*alpha_1
    g12 = (2*beta - 1)*q*mu_OUT*alpha_2
    g21 = (2*beta - 1)*q*mu_OUT*alpha_1
    g22 = (2*beta - 1)*p*mu_IN*alpha_2

    denom = (1 - g11)*(1 - g22) - g12*g21

    m1 = lambda_base*(1 - g22 + g12)/denom
    m2 = lambda_base*(1 - g11 + g21)/denom

    return m1, m2


def asymptotic_covariance(N, lambda_base, p, q, beta, alpha_1, mu_IN, mu_OUT):

    alpha_2 = 1 - alpha_1

    g11 = (2*beta - 1)*p*mu_IN*alpha_1
    g12 = (2*beta - 1)*q*mu_OUT*alpha_2
    g21 = (2*beta - 1)*q*mu_OUT*alpha_1
    g22 = (2*beta - 1)*p*mu_IN*alpha_2

    G = np.array([[g11, g12], [g21, g22]])
    I = np.eye(2)

    G_squared = np.dot(G, G)

    I_minus_G_squared = I - G_squared

    # Calculando a inversa de (I - G^2)
    inverse_I_minus_G_squared = np.linalg.inv(I_minus_G_squared)

    # Calculando ( (I - G^2)^-1 - I )
    result = inverse_I_minus_G_squared - I

    m = asymptotic_mean(lambda_base, p, q, beta, alpha_1, mu_IN, mu_OUT)[0]

    constant = 2*m*(1 - m)/N

    return constant*result[0, 0], constant*result[0,1]