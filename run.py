import argparse
from utils import getFoldersImage
from algo.dehazing import dehazingRaikwar, dehazingMeng, dehazingHe, dehazingCEP
from tqdm import trange
import cv2
import os

def main(args: argparse.ArgumentParser):
    paths = args.base_dir
    if paths == None:
        raise TypeError('None, please check the input!')
    
    paths = [paths] if isinstance(paths,str) else paths
    paths_data, paths_names = getFoldersImage(paths = paths)
    total_path_num = len(paths)
    with trange(total_path_num) as t:
        for idx_path in t:
            t.set_description(desc='Process %s [%d/%d] ' % (
                   paths[idx_path], idx_path+1, total_path_num))
            cur_path = paths[idx_path].replace('data','output/')     
            if not os.path.exists(cur_path):
                os.makedirs(cur_path)
            for idx, (data, name) in enumerate(zip(
                paths_data[idx_path], paths_names[idx_path])):
                t.set_postfix(name = name,)
                val = dehazingRaikwar(data)
                cv2.imwrite(f'{cur_path}/R_{name}',val)
                val = dehazingMeng(data)
                cv2.imwrite(f'{cur_path}/M_{name}',val)
                val = dehazingHe(data)
                cv2.imwrite(f'{cur_path}/H_{name}',val)
                
                val = dehazingCEP(data, is_fast = 1)
                cv2.imwrite(f'{cur_path}/CEPf_{name}',val)
                val = dehazingCEP(data, is_fast = 0)
                cv2.imwrite(f'{cur_path}/CEP_{name}',val)
            
             
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--base_dir',type=str,nargs='+',
                        default='./data')
    args_ = parser.parse_args()
    main(args = args_)