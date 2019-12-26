from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

img_serial = np.asarray(Image.open("hist_large.bmp"))
img_OpenCL = np.asarray(Image.open("opencl_hist_large.bmp"))
# plt.subplot(121), plt.imshow(img_serial), plt.title("Serial")
# plt.subplot(122), plt.imshow(img_OpenCL), plt.title("OpenCL")
# plt.show()
if np.abs(img_serial.astype(np.float64) - img_OpenCL.astype(np.float64)).max() > 0:
    print("Different!")
else:
    print("Exactly the same!")