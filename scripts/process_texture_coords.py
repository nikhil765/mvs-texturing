import glob
# from imp import source_from_cache
import os
import shutil
from argparse import ArgumentParser
import cv2
from ConcaveHull import ConcaveHull
import numpy as np
from sklearn.cluster import DBSCAN

# nvm_file = "Block/Block.nvm"

int_to_im_name = {}
boxes_per_im = {}

filenames = glob.glob("Block/images/*.jpg")
filenames.sort(key=lambda f: int(''.join(filter(str.isdigit, f))))

i = 1
for filename in filenames:
    split = filename.split("/")
    im_name = split[-1]
    int_to_im_name[i] = im_name
    i += 1

# print(int_to_im_name)

text_path = "test.txt"
im_folder = "BlockBoxedImages"

if not os.path.exists(im_folder):
    os.mkdir(im_folder)

with open(text_path, 'r') as f:
    lines = f.readlines()

for line in lines:
    line = line.strip()

    split = line.split(",")
    id = int(split[0])
    # min_y = int(split[1])
    # max_y = int(split[2])
    # min_x = int(split[3])
    # max_x = int(split[4])

    x = int(float(split[2]))
    y = int(float(split[1]))

    if id == 0:
        continue

    if id not in boxes_per_im:
        boxes_per_im[id] = [[x, y]]
    else:
        boxes_per_im[id].append([x, y])

for i in boxes_per_im:
    infos = boxes_per_im[i]
    curr_im_path = "Block/images/" + int_to_im_name[i]
    im = cv2.imread(curr_im_path)

    # print(i, ",", infos)
    # print(int_to_im_name[i])

    x_vals = []
    y_vals = []

    for curr_info in infos:
        # min_x = curr_info[0]
        # max_x = curr_info[1]
        # min_y = curr_info[2]
        # max_y = curr_info[3]

        x = curr_info[0]
        y = curr_info[1]

        x_vals.append(x)
        y_vals.append(y)

        # print(x, ",", y)

        im[y, x, :] = (255, 255, 0)
        
    pts = np.vstack([x_vals, y_vals])

    clustering = DBSCAN(eps=100, min_samples=1).fit(np.array([x_vals, y_vals]).T)
    labels = clustering.labels_
    unique_labels = np.unique(labels)

    for curr_label in unique_labels:
        cluster = pts[:, labels == curr_label]
        cluster = cluster.T

        ch = ConcaveHull()
        ch.loadpoints(cluster)
        ch.calculatehull(tol=100)

        boundary_points = np.vstack(ch.boundary.exterior.coords.xy).T
        print(boundary_points.shape)

        cv2.polylines(im, np.int32([boundary_points]), True, (0, 255, 0), 2)

    # for j in range(0, len(x_vals)):
        

    output_im_name = int_to_im_name[i]
    output_im_name = output_im_name[:output_im_name.find(".")]
    output_im_path = im_folder + "/" +  output_im_name + ".png"

    cv2.imwrite(output_im_path, im)

# print(boxes_per_im)