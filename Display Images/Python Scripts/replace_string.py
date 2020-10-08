import os
# Read in the file
with open('file.txt', 'r') as file :
  filedata = file.read()

# Replace the target string
filedata = filedata.replace("uint16_t image = {", "")
filedata = filedata.replace('0x', 'RC(0x')
filedata = filedata.replace(",", "),")
filedata = filedata.replace("};", "}")


# Write the file out again
with open('out.c', 'w') as file:
	file.write(""" const BITMAPSTRUCT name __attribute__((aligned)) =
{
  {
    0x4d42u,
    sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + (12 * 18 * 2),
    0x0000u,
    0x0000u,
    sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)
  },
  {
    sizeof(BITMAPINFOHEADER),
    12,
    18,
    1u,
    16,
    0x00000003u,
    (12 * 18 * 2),
    0x00000000ul,
    0x00000000ul,
    0x00000000ul,
    0x00000000ul
  },
  { 

          """ )

	file.write(filedata)

	file.write( """

	
}; 

	      """)

file.close()
