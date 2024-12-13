# Functions concerning correlation between neuronal firing

# Imports
import numpy as np
import matplotlib.pyplot as plt
from . import simulated_data
from . import errors

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


def theoretical_covariance(A, V, delay_1=False):
    """
    Solve the matrix equation for the vectorized Sigma,
    and transform it back into a (N, N) matrix.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N) used to compute the matrix L throught the `create_L_matrix` function
    V : numpy.ndarray
        A 2D array of shape (1, N) representing the variance of firings from each neuron.
    delay_1 : boolean
        Whether we are looking for a delay of 1 or not.

    Returns
    ----------
    numpy.ndarray
        A square matrix Sigma of shape (N, N) representing the covariance matrix between the neurons firing.
    """
    # Create matrix L using the function `create_L_matrix`
    L = create_L_matrix(A)

    # Get the size of matrix A
    N = A.shape[0]

    # Form the diagonal matrix DV using the variance vector
    DV = np.diag(V[0])

    # Flatten the diagonal matrix into a vector
    v = DV.flatten('F')

    # Create the identity matrix of size N^2 x N^2
    I = np.eye(N**2)

    # Compute the solution to the matrix equation
    sigma = np.matmul(np.linalg.inv(I - L), v)

    # Reshape the result back to a (N, N) matrix and return the transpose (`reshape` function will reshape the matrix row-wise instead of column-wise)
    sigma = sigma.reshape(N,N).T

    # For delay 1 covariance
    if delay_1:
        sigma_1 = np.matmul(A, sigma)
        return sigma_1
    else:
        return sigma
    

def plot_correlation_error(paths, labels, file_name, number_of_samples, delay_1):
    """
    Calculate and plot the absolute and relative absolute errors between
    theoretical and simulated correlations at different time steps.

    Parameters
    ----------
    paths : list of str
        A list of paths to directories containing sample data files.
    labels : list of str
        A list of labels to use as legend.
    file_name : str
        File name to save the plot generated.
    number_of_samples : int
        Number of samples in the experiment.
    delay_1 : boolean
        Whether we are looking for a delay of 1 or not.

    Returns
    ----------
    None
        This function does not return any value but generates, displays, and saves a plot.
        The plot shows the mean absolute errors and mean relative absolute errors
        with error bars indicating the standard deviation for each time step.
    """

    # Define a list of time steps `ts` to evaluate the error at
    ts = [5,6,7]
    xs = [10e5, 10e6, 10e7]

    # Define a colormap for consistent color assignment to paths
    cmap = plt.get_cmap("Dark2", len(paths))

    # Initialize data for both plots
    plot_data = {
        "absolute_error": {"means": [], "std_devs": []},
        "relative_absolute_error": {"means": [], "std_devs": []}
    }

    # Create a figure and axis object for the plot, set size and resolution
    fig, axs = plt.subplots(1, 2, figsize=(20,10), dpi=300)
    # Iterate over the 'paths' input
    for idx, path in enumerate(paths):
        # Initialize lists to store mean errors and standard deviation for each time step
        means_ae = []
        std_devs_ae = []
        means_rae = []
        std_devs_rae = []
        # Get the color for the current path and the proper label
        color = cmap(idx)
        label = labels[idx]
        # Iterate over each time step in `ts` 
        for t in ts:
            # Initialize a list to store the error for each of the samples
            errors_ae = []
            errors_rae = []
            # Perform the error calculation for each time step `t`
            for i in range(1, number_of_samples + 1):
                # Construct the path to the sample data
                sample = path + str(t) + "\\" + str(i)
                # Retrieve data for weights, variance, and correlation from the sample
                A = simulated_data.get_weight(sample)
                var = simulated_data.get_var(sample)
                simul_corr = simulated_data.get_corr(sample)
                # Solve the matrix equation to get the theoretical correlation
                theor_corr = theoretical_covariance(A, var, delay_1)/simulated_data.std_matrix(var)
                # Calculate the relative absolute error between the theoretical and simulated correlations
                error_ae = errors.absolute_error(theor_corr, simul_corr, exclude_diagonal=False)[0]
                error_rae = errors.absolute_error(theor_corr, simul_corr, exclude_diagonal=False)[1]
                # Append the error to the errors list
                errors_ae.append(error_ae)
                errors_rae.append(error_rae)
            # Compute the mean and standard deviation of the errors
            means_ae.append(np.mean(errors_ae))
            std_devs_ae.append(np.std(errors_ae))
            means_rae.append(np.mean(errors_rae))
            std_devs_rae.append(np.std(errors_rae))
        plot_data["absolute_error"]["means"].append((xs, means_ae, std_devs_ae, color, label))
        plot_data["relative_absolute_error"]["means"].append((xs, means_rae, std_devs_rae, color, label))

    # Plot absolute error
    fig, ax = plt.subplots(figsize=(10, 10), dpi=300)
    for xs, means, std_devs, color, label in plot_data["absolute_error"]["means"]:
        ax.plot(xs, means, marker='o', linestyle='--', color=color, label=label)
        ax.errorbar(xs, means, yerr=std_devs, fmt='o', capsize=5, color=color)

    ax.set_xlabel("Time")
    ax.set_xscale("log")
    ax.set_ylabel("Absolute Error")
    ax.set_yscale("log")
    ax.grid(True)
    ax.legend(loc="best")

    plt.savefig(file_name + "_absolute_error.pdf", format="pdf")
    plt.close()

    # Plot relative absolute error
    fig, ax = plt.subplots(figsize=(10, 10), dpi=300)
    for xs, means, std_devs, color, label in plot_data["relative_absolute_error"]["means"]:
        ax.plot(xs, means, marker='o', linestyle='--', color=color, label=label)
        ax.errorbar(xs, means, yerr=std_devs, fmt='o', capsize=5, color=color)

    ax.set_xlabel("Time")
    ax.set_xscale("log")
    ax.set_ylabel("Relative Absolute Error")
    ax.set_yscale("log")
    ax.grid(True)
    ax.legend(loc="best")

    plt.savefig(file_name + "_relative_absolute_error.pdf", format="pdf")
    plt.close()