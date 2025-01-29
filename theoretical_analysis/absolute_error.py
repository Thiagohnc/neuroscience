# Function to compute absolute error

# Imports
import numpy as np

def absolute_error(A, B, exclude_diagonal=False):
    """
    Compute the absolute error (AE) between two matrices element-wise
    and returns the mean of these errors.

    Parameters
    ----------
    A : numpy.ndarray
        The first matrix (ground truth or reference values).
    B : numpy.ndarray
        The second matrix (predicted or compared values).
    exclude_diagonal : bool, optional
        If True, excludes the main diagonal from the calculation (default is False).
    
    Returns
    ----------
    float
        The mean of the element-wise relative absolute errors.
    """

    # Ensure the inputs are numpy arrays
    A = np.asarray(A, dtype=float)
    B = np.asarray(B, dtype=float)

    # Check that A and B are of the same shape
    if A.shape != B.shape:
        raise ValueError("Matrices A and B must have the same shape.")
    
    # Compute the element-wise absolute difference
    absolute_error = np.abs(A - B)

    # Optionally exclude the main diagonal
    if exclude_diagonal:
        # Check if matrices are square
        if A.shape[0] != A.shape[1]:
            raise ValueError("Matrices must be square to exclude the main diagonal.")
        # Mask out the main diagonal
        diagonal_indices = np.diag_indices_from(A)
        absolute_error[diagonal_indices] = np.nan # Exclude diagonal by setting to NaN

    # Compute the mean AE, ignoring NaN values if diagonal is excluded
    return np.nanmean(absolute_error)