import cv2

def guidedFilter(est_t, gray_img, r, eps):
    '''
    :param est_t: input image
    :param gray_img: guidance image
    :param r: radius
    :param eps: eps
    :return: filtering output q
    K. He, J. Sun and X. Tang, "Guided Image Filtering," in IEEE 
    Transactions on Pattern Analysis and Machine Intelligence, 
    vol. 35, no. 6, pp. 1397-1409, June 2013, doi: 10.1109/TPAMI.2012.213.
    '''
    #1
    mean_I = cv2.boxFilter(gray_img, cv2.CV_64F, (r, r))
    mean_p = cv2.boxFilter(est_t, cv2.CV_64F, (r, r))
    corr_I = cv2.boxFilter(gray_img * gray_img, cv2.CV_64F, (r, r))
    corr_Ip = cv2.boxFilter(gray_img * est_t, cv2.CV_64F, (r, r))
    #2
    var_I = corr_I - mean_I * mean_I
    cov_Ip = corr_Ip - mean_I * mean_p
    #3
    a = cov_Ip / (var_I + eps)
    b = mean_p - a * mean_I
    #4
    mean_a = cv2.boxFilter(a, cv2.CV_64F, (r, r))
    mean_b = cv2.boxFilter(b, cv2.CV_64F, (r, r))
    #5
    q = mean_a * gray_img + mean_b
    return q