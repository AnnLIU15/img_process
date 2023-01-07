import numpy as np

def getFiltersKernel(kernel_type: int = 0) -> np.ndarray:
    r'''differential filters bank

    Args:
        kernel_type (int, optional): kernel type. Defaults to 0.

    Returns:
        np.ndarray: normalized filters
    '''
    if kernel_type == 0:    # NOTE: filter must have odd size to ensure the correct boundary
        kernel_weight = np.array([
        [[ 5,  5,  5], [-3,  0, -3], [-3, -3, -3]],
        [[-3,  5,  5], [-3,  0,  5], [-3, -3, -3]],
        [[-3, -3,  5], [-3,  0,  5], [-3, -3,  5]],
        [[-3, -3, -3], [-3,  0,  5], [-3,  5,  5]],
        [[ 5,  5,  5], [-3,  0, -3], [-3, -3, -3]],
        [[-3, -3, -3], [ 5,  0, -3], [ 5,  5, -3]],
        [[ 5, -3, -3], [ 5,  0, -3], [ 5, -3, -3]],
        [[ 5,  5, -3], [ 5,  0, -3], [-3, -3, -3]],
        [[ 0,  1,  0], [ 1, -4,  1], [ 0,  1,  0]],
        [[ 0, -1,  0], [-1,  4, -1], [ 0, -1,  0]],
            ],dtype = float)
    elif kernel_type == 1:  # KirschFilters Meng
        kernel_weight = np.array([
        [[-3, -3, -3], [-3, 0,  5], [-3,  5,  5]],
        [[-3, -3, -3], [-3, 0, -3], [ 5,  5,  5]],
        [[-3, -3, -3], [ 5, 0, -3], [ 5,  5, -3]],
        [[ 5, -3, -3], [ 5, 0, -3], [ 5, -3, -3]],
        [[ 5,  5, -3], [ 5, 0, -3], [-3, -3, -3]],
        [[ 5,  5,  5], [-3, 0, -3], [-3, -3, -3]],
        [[-3,  5,  5], [-3, 0,  5], [-3, -3, -3]],
        [[-3, -3,  5], [-3, 0,  5], [-3, -3,  5]],
        [[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]],
            ],dtype = float)
    else:
        pass
    for idx in range(kernel_weight.shape[0]):
        # print(kernel_weight[idx,...],np.linalg.norm(kernel_weight[idx,...].flatten()))
        kernel_weight[idx,...] /= np.linalg.norm(kernel_weight[idx,...].flatten())
    return kernel_weight

def unittest():
    data = getFiltersKernel()
    print(data[0,:,:],
          np.linalg.norm(data[0,:,:].flatten()),# vector norm
          np.linalg.norm(data[0,:,:]),          # vector norm
          np.linalg.norm(data[0,:,:], ord = 2), # matrix norm
          np.linalg.eigvals(data[0,:,:].T@data[0,:,:]))
if __name__ == '__main__':
    unittest()