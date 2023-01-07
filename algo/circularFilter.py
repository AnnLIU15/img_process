import cv2

def circularFilter(img, filter_kernel):
    filter_height, filter_width = filter_kernel.shape
    assert (filter_height == filter_width), 'Filter must be square in shape --> Height must be same as width'
    assert (filter_height % 2 == 1), 'Filter dimension must be a odd number.'

    filter_hals_size = int((filter_height - 1)/2)
    rows, cols = img.shape
    padded_img = cv2.copyMakeBorder(img, filter_hals_size, filter_hals_size, filter_hals_size, filter_hals_size, borderType=cv2.BORDER_WRAP)
    filtered_img = cv2.filter2D(padded_img, -1, filter_kernel)
    rt_val = filtered_img[filter_hals_size:rows+filter_hals_size, filter_hals_size:cols+filter_hals_size]

    return rt_val