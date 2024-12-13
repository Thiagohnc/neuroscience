# Functions concerning mean firing rate of neurons

# Imports
import numpy as np
import matplotlib.pyplot as plt
from . import simulated_data
from . import errors

def mean_firing(A, mu):
    """
    Compute the theoretical mean firing and theoretical variance for each neuron.

    Parameters
    ----------
    A : numpy.ndarray
        A square matrix of shape (N, N).
    mu : float
        Firing baseline, which represents the base activity level of neurons.

    Returns
    ----------
    numpy.ndarray
        The vector of theoretical mean firing for the neurons, and vector of theoretical variances.
    """
    
    # Get the number of neurons from the `A` matrix
    N = A.shape[0]

    # Create an identity matrix of size N x N
    I = np.eye(N)

    # Create a vector of ones of size N
    vector_1s = np.ones(N)

    # Compute the vector of mean firing for each neuron
    m = np.linalg.solve((I - A.T), mu*vector_1s)
    m = np.asarray(m)

    return m, (1-m)*m

def plot_mean_firing_error(paths, labels, file_name, number_of_samples, mu):
    """
    Calculate and plot the absolute and relative absolute errors between
    theoretical and simulated mean firing at different time steps.

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
    mu : float
        Firing baseline, which represents the base activity level of neurons.

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
                sample = path + str(t) + "\\" + str(i)
                # Retrieve data for weights, and mean firing from the sample
                A = simulated_data.get_weight(sample)
                simul_mean = simulated_data.get_avg(sample)[0]
                # Compute the theoretical mean firing
                theor_mean = mean_firing(A, mu)[0]
                # Calculate the error between the theoretical and simulated mean firing
                error_ae = errors.absolute_error(theor_mean, simul_mean, exclude_diagonal=False)[0]
                error_rae = errors.absolute_error(theor_mean, simul_mean, exclude_diagonal=False)[1]
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
    fig, ax = plt.subplots(figsize=(10,10), dpi=300)
    for xs, means, std_devs, color, label in plot_data["absolute_error"]["means"]:
        ax.plot(xs, means, marker='o', linestyle='--', color=color, label=label)
        ax.errorbar(xs, means, yerr=std_devs, fmt='o', capsize=5, color=color)
    
    ax.set_xlabel("Time")
    ax.set_xscale("log")
    ax.set_ylabel("Absolute error")
    ax.set_yscale("log")
    ax.grid(True)
    ax.legend(loc="best")

    plt.savefig(file_name + "_absolute_error.pdf", format="pdf")
    plt.close()

    # Plot relative absolute error
    fig, ax = plt.subplots(figsize=(10,10), dpi=300)
    for xs, means, std_devs, color, label in plot_data["relative_absolute_error"]["means"]:
        ax.plot(xs, means, marker='o', linestyle='--', color=color, label=label)
        ax.errorbar(xs, means, yerr=std_devs, fmt='o', capsize=5, color=color)
    
    ax.set_xlabel("Time")
    ax.set_xscale("log")
    ax.set_ylabel("Relative absolute error")
    ax.set_yscale("log")
    ax.grid(True)
    ax.legend(loc="best")

    plt.savefig(file_name + "_relative_absolute_error.pdf", format="pdf")
    plt.close()