# Flip the images horizontally and capitalize all the letters in filename.

import os
from PIL import Image, ImageOps

path = os.getcwd()
print(path)

for filename in os.listdir(path):
    if filename.endswith(".png"):   # Check could be .png or .PNG
        im = Image.open(filename)
        im_mirror = ImageOps.mirror(im)
        basename = os.path.splitext(filename)
        im_mirror.save((basename[0]).upper() + str(".png"))