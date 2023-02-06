import glob
# from imp import source_from_cache
import os
import shutil
from argparse import ArgumentParser
import cv2
from ConcaveHull import ConcaveHull
import numpy as np
from sklearn.cluster import DBSCAN

mask_folder = "SkydeployBoxedMasks/"
output_folder = "SkydeployBoxedImages/"

filenames = glob.glob("Skydeploy/images/*.JPG")
filenames.sort(key=lambda f: int(''.join(filter(str.isdigit, f))))
print(filenames)

for filename in filenames:
    split = filename.split("/")
    im_name = split[-1]
    im_name = im_name[:im_name.find(".")]

    mask_path = mask_folder + im_name + ".png"
    if not os.path.exists(mask_path):
        continue

    mask = cv2.imread(mask_path, cv2.IMREAD_GRAYSCALE)

    im = cv2.imread(filename)

    print(im_name)

    output_path = output_folder + im_name + ".png"

    _, mask = cv2.threshold(mask, 1, 255, 0)

    row_col = np.where(mask != 0)
    row_vals = row_col[0]
    col_vals = row_col[1]

    pts = np.array([col_vals, row_vals])

    try:
        clustering = DBSCAN(eps=100, min_samples=1).fit(pts.T)
    except:
        print("Clustering failed: ", im_name)
        continue
    labels = clustering.labels_
    unique_labels = np.unique(labels)

    for curr_label in unique_labels:
        cluster = pts[:, labels == curr_label]
        cluster = cluster.T

        ch = ConcaveHull()
        ch.loadpoints(cluster)
        try:
            ch.calculatehull(tol=100)
        except:
            print("Failed on this point set: ", cluster.shape)
            continue

        boundary_points = np.vstack(ch.boundary.exterior.coords.xy).T
        # print(boundary_points.shape)

        cv2.polylines(im, np.int32([boundary_points]), True, (0, 255, 0), 2)

    cv2.imwrite(output_path, im)