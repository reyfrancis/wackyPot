''' 
This will flip horizontally an image array. The width and the height must be correctly specified in the const BITMAPSTRUCT file, 
in this specific line of code:

sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + ({width} * {height} * 2),

author: Rey Francis Famulagan 
'''

#Example Usage: python flip_pixels.py -f flip.txt

import os
import fileinput
import argparse


def flipImageArray(filename):

  hex_list = []
  hex_list_flip = []
  hex_start = 0
  hex_count = 0
  copy_start = 0

  with fileinput.input("flip.txt") as fp:
    line = fp.readline()
    while line:
      line = fp.readline()

      if 'const BITMAPSTRUCT' in line:
        copy_start = fp.lineno() # Mark where the line where we start copying

      if 'sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +' in line:
        c = line
        # Check how many digits is width value
        init_str = len("    sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + (")
        width_dig = 0
        while c[init_str + width_dig] != '*': # Count until the asterisk sign
          width_dig += 1
        width_dig -= 1 # Subtract the space character
        width = int(c[init_str: init_str + width_dig])

        height_dig = 0

        while c[init_str + width_dig + 3 + height_dig] != ' ': # Count until the asterisk sign
          height_dig += 1
        height = int(c[init_str + width_dig + 3: init_str + width_dig + 3 + height_dig])

      if 'RC(0x' in line:

        if hex_start == 0:
          hex_start =  fp.lineno()

        hex_count += 1
        each_line = line.split(",")
        for i in range(len(each_line)):
          hex_list.append(each_line[i].strip())
      else:
        hex_end = 0 # hex_start + hex_count

    for j in range(len(hex_list)-hex_count): # Subtract 5 since we are deleting each element while looping
      if hex_list[j] == '':
        hex_list.pop(j)

    for l in range(height):
      k = 0
      while k < width:
        hex_list_flip.append(hex_list[width*(l+1) - k - 1])
        k += 1

  return hex_start, copy_start, filename, hex_list_flip

def saveImageArray(output_name, *args):

  fp_copy = open(args[2])
  lines=fp_copy.readlines()
  with open(output_name, 'w') as g:             
    for x in range(args[0] - args[1] - 1):
      g.write(lines[x])

    for y in range(args[0]):
      for i in range(16):
        if y*16+i < len(args[3]):            # Check if we are not out of index

          if y*16+i == (len(args[3])-1):     # Check if we are already at the last set of line and last element
            g.write(args[3][y*16+i])         # Do not include a comma for the very last character
          else:

            if i == 15:                        # Check if we are already adding the 16th element for this line
              g.write(args[3][y*16+i] + ",\n") # If this is the 16th line, we add a newline character
            else:
              g.write(args[3][y*16+i] + ", ") 

        else:
          break
          # Do nothing
    g.write( " } };")
    # g.close()

if __name__ == '__main__':
  # Command-line arguments
  parser = argparse.ArgumentParser(
      description='''This will flip horizontally an image array. The width and the height must be correctly specified in the const BITMAPSTRUCT file, 
in this specific line of code:

sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + ( width * height * 2),
''')

  parser.add_argument('-f', '--filename',
                      type=str,
                      help='Filename [str]',
                      required=True)
  parser.add_argument('-o', '--output_name',
                      type=str,
                      help='Ouput filename [str]',
                      default="OUT.txt",
                      required=False)
  args = parser.parse_args()
  mylist = flipImageArray(args.filename)
  saveImageArray(args.output_name, *mylist) # Whenever you pass a list into a function, 
  #it is converted into a tuple. Then you need to unpack in the function call