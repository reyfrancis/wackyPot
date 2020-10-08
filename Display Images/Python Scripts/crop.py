# Improting Image class from PIL module 
from PIL import Image 
  
# Opens a image in RGB mode 
im = Image.open(r"E:\User\Desktop\LCD_Updates\numbers1.png") 
  
# Size of the image in pixels (size of orginal image) 
# (This is not mandatory) 
width, height = im.size 

mylist = [54, 99, 143, 187, 231]

# Setting the points for cropped image
width = 12
height = 18

for i in range(len(mylist)):
    for k in range(0, 2):
        left = mylist[i] + k*12
        top = 140

        right = left + width
        bottom = top + height

        # Cropped image of above dimension 
        # (It will not change orginal image) 
        im1 = im.crop((left, top, right, bottom)) 
        im1 = im1.save(str( 2*i+1+k ) +"_66_g.png") 
