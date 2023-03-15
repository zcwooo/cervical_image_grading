"""
File:progresstest.py
Date: 2021/4/5 11:18
Auther:Affreden
"""
from progressbar import *
import time
from tqdm import tqdm

int_dir = 'C://Users//Administrator//Pictures//test_lzsp'
img_names = os.listdir(int_dir)
for img_name in img_names:
    print(img_name)
    if not (img_name.endswith('.jpg') or img_name.endswith('.png')):
        continue
    print(img_name)

