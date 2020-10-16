''' 
Write the proper const BITMAPSTRUCT initialization with an image array.
author: Rey Francis Famulagan 
'''

# Example Usage: python replace_string.py -f file.txt -w 100 -l 50 -n SLP_SLP_0

import os
import argparse

def replaceString(filename, _width, _height, _name, output_name):

  # Read the file
  with open(filename, 'r') as file :
    filedata = file.read()

  # Replace the target string
  filedata = filedata.replace("uint16_t image = {", "")
  filedata = filedata.replace('0x', 'RC(0x')
  filedata = filedata.replace(",", "),")
  filedata = filedata.replace("};", "}")

  # Write the output file
  with open(output_name, 'w') as file:
  	file.write(''' const BITMAPSTRUCT {name} __attribute__((aligned)) =
  {{
    {{
      0x4d42u,
      sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + ({width} * {height} * 2),
      0x0000u,
      0x0000u,
      sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)
    }},
    {{
      sizeof(BITMAPINFOHEADER),
      {width},
      {height},
      1u,
      16,
      0x00000003u,
      ({width} * {height} * 2),
      0x00000000ul,
      0x00000000ul,
      0x00000000ul,
      0x00000000ul
    }},
    {{ 

            '''.format(width = _width, height = _height, name = _name))

  	file.write(filedata)

  	file.write('''

  	
  }; 

  	      ''')

  file.close()


if __name__ == '__main__':
    # Command-line arguments
    parser = argparse.ArgumentParser(
        description='Write the proper const BITMAPSTRUCT initialization with an image array.')

    parser.add_argument('-f', '--filename',
                        type=str,
                        help='Filename [str]',
                        required=True)
    parser.add_argument('-w', '--width',
                        type=int,
                        help='Width of the image [int]',
                        required=True)
    parser.add_argument('-l', '--height',
                        type=int,
                        help='Length or height of the image [int]',
                        required=True)
    parser.add_argument('-n', '--name',
                        type=str,
                        help='Name of the image [str]',
                        required=True)
    parser.add_argument('-o', '--output_name',
                        type=str,
                        help='Ouput filename [str]',
                        default="OUT.c",
                        required=False)

    args = parser.parse_args()
    replaceString(args.filename, args.width, args.height, args.name, args.output_name)