''' 
This will flip all the images in the given folder horizontally.
author: Rey Francis Famulagan 
'''

# Example Usage: python flip_horizontal.py -f Cropped_Images -x png

# Imports
import os
from PIL import Image, ImageOps
import argparse

def flipHorizontally(folder_name, extension):
    path = os.getcwd()
    for filename in os.listdir(path+'/'+folder_name):
        print(filename)
        if filename.endswith("."+extension):  
            im = Image.open(path+'/'+folder_name+'/'+filename)
            im_mirror = ImageOps.mirror(im)
            basename = os.path.splitext(filename)
            im_mirror.save(path+'/'+folder_name+'/'+(basename[0]).upper() + str("."+extension))

if __name__ == '__main__':
    # Command-line arguments
    parser = argparse.ArgumentParser(
        description='Flip horizontally all the images in a given folder')

    parser.add_argument('-f', '--folder_name',
                        type=str,
                        help='Name of the folder where the images are stored [str]',
                        required=True)
    parser.add_argument('-x', '--extension',
                        type=str,
                        help='File extension of the image. Could be png, jpg, or bmp [str]',
                        default="png",
                        required=False)
    args = parser.parse_args()
    flipHorizontally(args.folder_name, args.extension)