#ifndef ROT_ENCODER_H
#define ROT_ENCODER_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes */
#include <stdint.h> /* Necessary for uint8_t, uint16_t, etc data types */

typedef struct 
{ 
  int8_t increment;
  uint8_t click; 
}Encoder_valTypeDef;

/* Global Function Prototypes */
void Enc_Init(void);
Encoder_valTypeDef Enc_GetIncrement(uint8_t click_val);

#endif /*ROT_ENCODER_H */
