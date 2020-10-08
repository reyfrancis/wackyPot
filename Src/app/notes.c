/* Measured according to the picture

Radius = 92 px; (Applied Pythagorean Theorem)
Arc = 203 deg;
Spaces = 8; (8 spaces for nine items distributed evenly)
theta = interval = 203/8 = 25.375

To optimize the code, we will not include <cmath> library. Hence, the values are computed on Visual Code Studio.
The following code prints the coordinates in double data type.

#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include  <cmath>

int main(){
double x = 67;
double y = 101;
double cx = 160;
double cy = 120;
double theta =  (double)i*25.375*3.14159/180;
double x_trans; 
double y_trans;


int i = 8; // Modify to get the coordinates of specific item

x_trans = ((x - cx)* cos(theta) + (y-cy)* sin(theta)) + cx;
y_trans = (-(x - cx)* sin(theta) + (y-cy)* cos(theta)) + cy; 
printf ("Decimals: %f %f \n", x_trans, y_trans);
printf ("Decimals: %f \n", theta);

  return 0;
}   

*/