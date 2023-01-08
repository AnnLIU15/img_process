import numpy as np


# a = (1/8)**0.5
# b = 0.5*np.cos(np.pi/8)
# c = 0.5*np.cos(np.pi/8*3)
# d = 0.5*np.cos(np.pi/16)
# e = 0.5*np.cos(np.pi/16*3)
# f = 0.5*np.cos(np.pi/16*5)
# g = 0.5*np.cos(np.pi/16*7)


# a = np.array([[ a,  a,  a,  a,  a,  a,  a,  a],
#               [ d,  e,  f,  g, -g, -f, -e, -d],
#               [ b,  c, -c, -b, -b, -c,  c,  b],
#               [ e, -g, -d, -f,  f,  d,  g, -e],
#               [ a, -a, -a,  a,  a, -a, -a,  a],
#               [ f, -d,  g,  e, -e, -g,  d, -f],
#               [ c, -b,  b, -c, -c,  b, -b,  c],
#               [ g, -f,  e, -d,  d, -e,  f, -g],
#           ])
a = np.array([[64, 64, 64, 64, 64, 64, 64, 64,],
              [89, 75, 50, 18, -18,-50, -75, -89],
              [83, 36, -36, -83, -83, -36, 36, 83],
              [75, -18, -89,-50, 50, 89, 18, -75],
              [64, -64,-64,64,64,-64,-64,64],
              [50,-89,18,75,-75,-18,89,-50],
              [36,-83,83,-36,-36,83,-83,36],
              [18,-50,75,-89,89,-75,50,-18],
])

def cal_trans(mat):
    row, col = mat.shape
    rt_mat = np.zeros((row, col))
    rt_mat_2 = np.zeros((row, col))
    for idx_row in range(row):
        for idx_col in range(col):
            rt_mat_2[idx_row, idx_col]=np.sum(mat[idx_row,:])*np.sum(mat[idx_col,:])
            rt_mat[idx_row, idx_col] = 1/np.sqrt(
                np.sum(mat[idx_row,:])*np.sum(mat[idx_col,:])
            )
            
    return rt_mat


print(a)
print(a.T)

print(cal_trans(a**2).astype(int),np.unique(cal_trans(a**2)))

