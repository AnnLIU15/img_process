# Single-Image-Dehazing

## Author

ZhaoY emali: [liuzhy86@mail2.sysu.edu.cn](mailto:liuzhy86@mail2.sysu.edu.cn)

## Reproduction algorithms

* Python version of "Lower Bound on Transmission Using Non-Linear Bounding Function in Single Image Dehazing[^1]".
* "Single Image Haze Removal Using Dark Channel Prior[^2]"
* "Single Image Dehazing Using Color Ellipsoid Prior[^3]"
* "Efficient Image Dehazing with Boundary Constraint and Contextual Regularization[^4]"

## How to run

Download the dataset from [RESIDE](https://sites.google.com/view/reside-dehaze-datasets/reside-standard)

unzip to `data` folder and run

```
python run.py --base_dir data/xxxx	# save the result to output folder without metrics

# or

python run_metric.py --base_dir data/xxxx	# don't save the result and with metrics
```

## Environment

* Python3.7(conda)
* Dependency: please read [requirements.txt](requirements.txt) files.

```bash
pip install -r requirements.txt
```

[^1]: S. C. Raikwar and S. Tapaswi, "Lower Bound on Transmission Using Non-Linear Bounding Function in Single Image Dehazing," in IEEE Transactions on Image Processing, vol. 29, pp. 4832-4847, 2020, doi: 10.1109/TIP.2020.2975909.
    
[^2]: K. He, J. Sun, and X. Tang, “Single Image Haze Removal Using Dark Channel Prior,”  *Ieee T Pattern Anal* , vol. 33, no. 12, pp. 2341–2353, Dec. 2011, doi: [10.1109/TPAMI.2010.168](https://doi.org/10.1109/TPAMI.2010.168).
    
[^3]: T. M. Bui and W. Kim, “Single Image Dehazing Using Color Ellipsoid Prior,” *Ieee T Image Process*, vol. 27, no. 2, pp. 999–1009, Feb. 2018, doi: [10.1109/TIP.2017.2771158](https://doi.org/10.1109/TIP.2017.2771158).
    
[^4]: G. Meng, Y. Wang, J. Duan, S. Xiang, and C. Pan, “Efficient Image Dehazing with Boundary Constraint and Contextual Regularization,” in *2013 IEEE International Conference on Computer Vision*, Dec. 2013, pp. 617–624. doi: [10.1109/ICCV.2013.82](https://doi.org/10.1109/ICCV.2013.82).
