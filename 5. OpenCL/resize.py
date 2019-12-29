from PIL import Image  
import PIL  
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--img_name", default="airplane")
parser.add_argument("--height", default=480, type=int)
parser.add_argument("--width", default=720, type=int)
parser.add_argument("--save_name", default="small")
args = parser.parse_args()

img = Image.open("{}.bmp".format(args.img_name))
img_resize = img.resize((args.width, args.height))
img_resize.save("{}.bmp".format(args.save_name))