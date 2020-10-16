#ifndef ROT_ENCODER_H
#define ROT_ENCODER_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes */
#include <stdint.h> /* Necessary for uint8_t, uint16_t, etc data types */

/* Absolute value function */	
#define ABS_VAL(x) (x > 0 ? x : -x)

/* CLICK values */	
#define CLICK 			   1

/* Increment Minimum registered value */	
#define MIN_INCREMENT 	   3



typedef struct 
{ 
  int8_t increment;
  uint8_t click; 
}Encoder_valTypeDef;

/* Global Function Prototypes */
void Enc_Init(void);
int8_t Enc_GetIncrement(void);
uint8_t Enc_getClick(void);

#ifdef __cplusplus
}
#endif

#endif /*ROT_ENCODER_H */
