# -------------------------------------#
#       对多张图片进行预测
# -------------------------------------#
from yolo import YOLO
from PIL import Image
import os


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
int_dir = r'G:\scop_pic\img'
save_path = 'detected_results/results20201202/'
cut_results = save_path + "cut_results/"
if not os.path.exists(cut_results):
    os.makedirs(cut_results)
if not os.path.exists(save_path):
    os.makedirs(save_path)
img_names = os.listdir(int_dir)
summary = {}
path = ""
for img_name in img_names:
    # img = input('Input image filename:')
    if not img_name.endswith('.jpg'):
        continue
    img_path = os.path.join(int_dir, img_name)
    print(img_name)
    try:
        image = Image.open(img_path)
        image_copy = image.copy()
    except:
        print('Open Image Error! Try again!')
        continue
    else:
        r_image, temp, anchors = yolo.detect_image(image)
        """
        把裁剪出来的单细胞单独保存
        """
        # for anchor in anchors:
        #     # temp=anchors.index(anchor)
        #     crop_name=img_name.strip(".jpg")+f"_{anchors.index(anchor)}.jpg"
        #     image_copy.crop(anchor).save(save_path + f"cut_results/{crop_name}")

        summary = sum_dict(summary, temp)
        model = yolo.__dict__["model_path"].split('/')[-1].strip('\n')
        path = save_path + model
        if not os.path.exists(path):
            os.makedirs(path)
        r_image = r_image.convert('RGB')
        r_image.save(f"{path}/{img_name}")
with open(f"{path}/summary.txt", "w") as sum_file:
    for key in summary.keys():
        sum_file.write(f"{key} {summary[key]}\n")
