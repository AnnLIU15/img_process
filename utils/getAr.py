# from scipy.ndimage import minimum_filter
import numpy as np
import cv2

def getAr(hazy_img: np.ndarray,
          normalized: bool = False,
          windowSize: int = 3) -> np.ndarray:
    r'''calculate the atmospheric light from hazy image,
    G. Meng, Y. Wang, J. Duan, S. Xiang and C. Pan, "Efficient
    Image Dehazing with Boundary Constraint and Contextual Regularization,"
    2013 IEEE International Conference on Computer Vision, 2013, pp. 617-624.

    Args:
        hazy_img (np.ndarray): hazy image
        normalized (bool): set to 1?
        windowSize (int): filter kernel size
        
    Returns:
        np.ndarray: atmospheric light A
    '''
    _, _, dims = hazy_img.shape
    Ar = np.zeros(dims)
    kernel = np.ones((windowSize, windowSize), np.uint8)
    for dim in range(dims):
        cur_channel = hazy_img[:,:,dim]
        # min_filter_val = minimum_filter(cur_channel, size = (windowSize, windowSize), mode = 'mirror')
        min_filter_val = cv2.erode(cur_channel, kernel)
        Ar[dim] = np.max(min_filter_val)
    Ar = Ar/255 if normalized else Ar
    return Ar

def getArHe(hazy_img: np.ndarray,
            windowSize: int = 15,
            percent: float = 1e-3) -> np.ndarray:
    r'''calculate the atmospheric light from hazy image,
    K. He, J. Sun and X. Tang, "Single Image Haze Removal Using Dark
    Channel Prior," in IEEE Transactions on Pattern Analysis and
    Machine Intelligence, vol. 33, no. 12, pp. 2341-2353, 
    Dec. 2011, doi: 10.1109/TPAMI.2010.168.

    Args:
        hazy_img (np.ndarray): hazy image(normalized)
        windowSize (int): filter kernel size
        percent (float): percent to cal Ar
        
    Returns:
        float: atmospheric light A
    '''
    dark_channel = getDarkChannel(hazy_img, windowSize=windowSize)
    row, col, dims = hazy_img.shape
    img_shape = row * col
    num_of_pixel = int(np.maximum(np.floor(img_shape * percent),1))
    vec_dark_channel = dark_channel.flatten()
    vec_img = hazy_img.reshape(img_shape, dims)
    indices = vec_dark_channel.argsort()
    indices = indices[img_shape-num_of_pixel::]
    
    
    Ar = np.zeros((1,dims))
    for idx in range(num_of_pixel):
        Ar += vec_img[indices[idx]]
    Ar = Ar.flatten()/num_of_pixel

    return Ar

def getDarkChannel(hazy_img: np.ndarray, # normalized
                   windowSize: int = 15,) -> np.ndarray:
    r'''calculate the dark channel from hazy image,
    K. He, J. Sun and X. Tang, "Single Image Haze Removal Using Dark
    Channel Prior," in IEEE Transactions on Pattern Analysis and
    Machine Intelligence, vol. 33, no. 12, pp. 2341-2353, 
    Dec. 2011, doi: 10.1109/TPAMI.2010.168.

    Args:
        hazy_img (np.ndarray): hazy image(normalized)
        windowSize (int): filter kernel size
        
    Returns:
        np.ndarray: dark channel of A
    '''
    min_I = np.min(hazy_img, axis = -1)
    kernel = np.ones((windowSize, windowSize), np.uint8)
    dark = cv2.erode(min_I,kernel)
    return dark


def getArCEP(hazy_img: np.ndarray,
            windowSize: int = 15,
            percent: float = 1e-3) -> np.ndarray:
    r'''calculate the atmospheric light from hazy image,
    T. M. Bui and W. Kim, "Single Image Dehazing Using Color 
    Ellipsoid Prior," in IEEE Transactions on Image Processing,
    vol. 27, no. 2, pp. 999-1009, Feb. 2018, doi: 10.1109/TIP.2017.2771158.

    Args:
        hazy_img (np.ndarray): hazy image(normalized)
        windowSize (int): filter kernel size
        percent (float): percent to cal Ar
        
    Returns:
        float: atmospheric light A
    '''
    row, col, dims = hazy_img.shape
    img_shape = row * col
    wS = max(4 * windowSize + 1, 61)
    min_I = np.min(hazy_img, axis=-1)
    mean_min_I = cv2.boxFilter(min_I, -1, (wS, wS))
    mean_min_I = mean_min_I.flatten()
    argsort = np.argsort(mean_min_I)[::-1]
    air_locs_1D = argsort[:int(img_shape * percent)]
    air_Y = air_locs_1D // col
    air_X = air_locs_1D - air_Y * col
    air_locs = (air_Y, air_X)
    Ar = np.zeros(dims)
    for idx in range(dims):
        Ar[idx] = np.mean(hazy_img[..., idx][air_locs])
    return Ar