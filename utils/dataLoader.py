from glob import glob
import cv2
from typing import Union, List, Tuple

def getFoldersImage(paths: Union[List[str],str],
                    pic_types: Union[List[str],str] 
                    = ['.jpg','.jpeg', '.png'],
                    ) -> Tuple[List[list],List[list]]:
    r''' get all `pic_types` which be assigned in the folders

    Args:
        paths (Union[List[str],str]): relative/absolute paths of dataset
        pic_types (Union[List[str],str], optional): file extensions. Defaults to ['.jpg','.jpeg', '.png'].

    Returns:
        Tuple[List[list],List[list]]: data list and names list
    '''
    paths_data, paths_names = [],[]
    if paths == None:
        print('None, please check the input!')
    else:
        paths = [paths] if isinstance(paths,str) else paths
        # length_paths = len(paths)
        for idx, dataset_path in enumerate(paths):
            # print(f'[{idx+1}|{length_paths}]Current loading: {dataset_path}')
            pic_data, pic_name = getImage(dataset_path, pic_types)
            paths_data.append(pic_data)
            paths_names.append(pic_name)
    return paths_data, paths_names

def getImage(dataset_path: str,
             pic_types: Union[List[str],str]='.jpg',
             ) -> Tuple[list,list]:
    r'''
    get all `pic_types` which be assigned in the folder

    Args:
        dataset_path (str): relative/absolute path of dataset
        pic_types (Union[List[str],str], optional): file extensions. Defaults to '.jpg'.

    Returns:
        Tuple[list,list]: data list and names list
    '''
    pic_types = [pic_types] if isinstance(pic_types,str) else pic_types
    pics = []
    for pic_type in pic_types:
        pics.extend(sorted(glob(dataset_path+'/*'+pic_type)))
    pic_data = []
    pic_name = []
    # length_path = len(dataset_path)+1
    for pic in pics:
        pic_ = cv2.imread(pic, cv2.IMREAD_COLOR)
        # print(pic[length_path:], pic_.shape)

        pic_data.append(pic_), pic_name.append(pic.replace('\\','/').split('/')[-1])
    return pic_data, pic_name

def unittest(paths):
    paths_data, paths_names = getFoldersImage(paths)
    print(paths_names)

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--base_dir',type=str,nargs='+')
    args = parser.parse_args()
    unittest(paths=args.base_dir)