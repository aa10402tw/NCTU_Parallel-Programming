from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--img_dir", default=".")
parser.add_argument("--img_name", default="airplane")
args = parser.parse_args()

print("Compare [{}] and [{}] ...".format(
	"serial_hist_{}.bmp".format(args.img_name), 
	"opencl_hist_{}.bmp".format(args.img_name))
)
img_serial = np.asarray(Image.open("{}/serial_hist_{}.bmp".format(args.img_dir, args.img_name)))
img_OpenCL = np.asarray(Image.open("{}/opencl_hist_{}.bmp".format(args.img_dir, args.img_name)))
if np.abs(img_serial.astype(np.float64) - img_OpenCL.astype(np.float64)).max() > 0:
    print("[Error] Serial & OpenCL result are different!")
else:
    print("[Correct] Serial & OpenCL result are exactly the same!")