# -------------------------------------#
#       对多张图片进行预测
# -------------------------------------#
from yolo import YOLO
from PIL import Image
import os
import json
from progressbar import *


def sum_dict(a, b):
    temp = dict()
    # python3,dict_keys类似set； | 并集
    for key in a.keys() | b.keys():
        temp[key] = sum([d.get(key, 0) for d in (a, b)])
    return temp


yolo = YOLO()
# listdir=input("input image dirtxt")
# listdir="xsp_train.txt"
# imgdirs=open(listdir,'r').readlines()
int_dir = '{0}'
save_path = 'result//{1}'
cut_results = save_path + "/cut_results/"
if not os.path.exists(cut_results):
    os.makedirs(cut_results)
if not os.path.exists(save_path):
    os.makedirs(save_path)
img_names = os.listdir(int_dir)
print(f'一共有{img_names.__len__()}张图片')
summary = {}
path = ""
widgets = ['Progress: ', Percentage(), ' ', Bar('#'), ' ', FileTransferSpeed(), ' ']
progress = ProgressBar(widgets=widgets)

try:
    sum_file = open(f"{save_path}/summary.txt", "w")
    for img_name in progress(img_names):
        # img = input('Input image filename:')
        if not (img_name.endswith('.jpg') or img_name.endswith('.png')):
            continue
        img_path = os.path.join(int_dir, img_name)
        # print(img_name)
        try:
            image = Image.open(img_path)
            image_copy = image.copy()
        except Exception as er:
            print(f'Open Error! Try again! {er}')
            continue
        else:
            sum_file.write(img_path + ', ')
            r_image, temp, anchors = yolo.detect_image(image)
            """
            把裁剪出来的单细胞单独保存
            """
            for anchor in anchors:
                # temp=anchors.index(anchor)
                crop_name = img_name.strip(".jpg") + f"_{anchors.index(anchor)}.jpg"
                image_copy.crop(anchor[1]).save(save_path + f"/cut_results/{crop_name}-{anchor[0]}")
                sum_file.write(anchor[0]+f' {anchor[1][0]} {anchor[1][1]} {anchor[1][2]} {anchor[1][3]},')
            summary = sum_dict(summary, temp)
            model = yolo.__dict__["model_path"].split('/')[-1].strip('\n')
            r_image = r_image.convert('RGB')
            r_image.save(f"{save_path}/{img_name}")
            sum_file.write('\n')
    sum_file.close()
    with open(os.path.join(save_path, 'status.txt'), 'w') as f:
        f.write('ok')
except Exception as err:
    print(err)
    with open(os.path.join(save_path, 'status.txt'), 'w') as f:
        f.write('error')