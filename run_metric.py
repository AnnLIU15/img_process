import argparse
from utils import getFoldersImage
from algo.dehazing import dehazingRaikwar, dehazingMeng, dehazingHe, dehazingCEP
from tqdm import trange
import cv2
import os
from skimage.metrics import structural_similarity as ssim
from skimage.metrics import peak_signal_noise_ratio as psnr
import numpy as np
def main(args: argparse.ArgumentParser):
    paths = args.base_dir
    if paths == None:
        raise TypeError('None, please check the input!')
    gt_paths = [path + '/gt/' for path in paths]
    hazy_paths = [path+ '/hazy/' for path in paths]
    
    gt_paths_data, gt_paths_names = getFoldersImage(paths = gt_paths)
    paths_data, paths_names = getFoldersImage(paths = hazy_paths)
    
    total_path_num = len(paths)
    with trange(total_path_num) as t:
        for idx_path in t:
            t.set_description(desc='Process %s [%d/%d] ' % (
                   paths[idx_path], idx_path+1, total_path_num))
            cur_path = paths[idx_path].replace('data','output/')     
            # if not os.path.exists(cur_path):
            #     os.makedirs(cur_path)
            
            metric = np.zeros((5,len(paths_names[idx_path]),2))
            cur_pos = 0
            for idx, (data, name) in enumerate(zip(
                paths_data[idx_path], paths_names[idx_path])):
                t.set_postfix(name = name,)
                base_name = name.split('_')[0]
                while not base_name in gt_paths_names[idx_path][cur_pos]:
                    cur_pos += 1
                
                # print(cur_pos, gt_paths_names[idx_path][cur_pos],name)
                uint8_data = np.uint8(gt_paths_data[idx_path][cur_pos])
                val = dehazingRaikwar(data)
                metric[0,idx,:] = [psnr(uint8_data,np.uint8(val)),
                     ssim(uint8_data,np.uint8(val),channel_axis = -1)]
                # cv2.imwrite(f'{cur_path}/R_{name}',val)
                val = dehazingMeng(data)
                metric[1,idx,:] = [psnr(uint8_data,np.uint8(val)),
                     ssim(uint8_data,np.uint8(val),channel_axis = -1)]
                
                # cv2.imwrite(f'{cur_path}/M_{name}',val)
                val = dehazingHe(data)
                metric[2,idx,:] = [psnr(uint8_data,np.uint8(val)),
                     ssim(uint8_data,np.uint8(val),channel_axis = -1)]
                # cv2.imwrite(f'{cur_path}/H_{name}',val)
                
                val = dehazingCEP(data, is_fast = 1)
                metric[3,idx,:] = [psnr(uint8_data,np.uint8(val)),
                     ssim(uint8_data,np.uint8(val),channel_axis = -1)]
                # cv2.imwrite(f'{cur_path}/CEPf_{name}',val)
                val = dehazingCEP(data, is_fast = 0)
                metric[4,idx,:] = [psnr(uint8_data,np.uint8(val)),
                     ssim(uint8_data,np.uint8(val),channel_axis = -1)]
                
            
            avg_metric = np.mean(metric,axis=1)
            psnr_val_avg = avg_metric[:,0]
            ssim_val_avg = avg_metric[:,1]
            
            print(f' {paths[idx_path]} psnr:',
                  psnr_val_avg.flatten())
            print(f'{paths[idx_path]} ssim:',
                  ssim_val_avg.flatten())
            
             
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--base_dir',type=str,nargs='+',
                        default='./data')
    args_ = parser.parse_args()
    main(args = args_)