''' 
Crop the image by inputting dimensions and top left coordinates.
author: Rey Francis Famulagan 
'''

# Example Usage: python crop.py --c 10 10 -w 60 -l 150 -n filename.png

# Imports
from PIL import Image 
import os  
import argparse

def cropImage(left, top, width, height, name, output_name):
    path = os.getcwd()
    im = Image.open(path+'/'+name) 
    right = left + width
    bottom = top + height

	# Cropped image of above dimension. This will not change the original image.
    im1 = im.crop((left, top, right, bottom)) 
    im1 = im1.save(args.output_name) 

if __name__ == '__main__':
    # Command-line arguments
    parser = argparse.ArgumentParser(
        description='Crop the image by inputting dimensions and top left coordinates')

    parser.add_argument('-c', '--coordinates',
    					nargs=2,
                        type=int,
                        help='Top left (x, y) coordinates [int]',
                        required=True)
    parser.add_argument('-w', '--width',
                        type=int,
                        help='Width of the cropped image [int]',
                        required=True)
    parser.add_argument('-l', '--length',
                        type=int,
                        help='Length or height of the cropped image [int]',
                        required=True)
    parser.add_argument('-f', '--filename',
                        type=str,
                        help='Filename [str]',
                        required=True)
    parser.add_argument('-o', '--output_name',
                    type=str,
                    help='Ouput filename [str]',
                    default="OUT.png",
                    required=False)
    args = parser.parse_args()
    cropImage(args.coordinates[0], args.coordinates[1], args.width, args.length, args.filenames, args.output_name)