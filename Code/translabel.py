"""
转换标签形式
"""

import os

# imgdir = 'training_data/lzsp_data/lzsp_train'
img_dir= r'G:\scop_pic\608_img_s'
files = os.listdir(img_dir)
savetxt = open('lzsp_train20201202.txt', 'w')
for file in files:
    if not file.endswith('jpg'):
        continue
    img_id = file.split('.')[0]
    txt = open(img_dir + '/' + file.replace('.jpg', '.txt'))
    lines = txt.readlines()
    savetxt.write(os.path.join(img_dir, file))
    for line in lines:
        savetxt.write(' ' + line.split()[1] + ',' + line.split()[2] + ',' + line.split()[3] + ',' +
                      line.split()[4] + ',' + line.split()[0])
    savetxt.write('\n')
