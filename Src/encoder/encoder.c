/* Includes */
#include "encoder.h"
#include "main.h"

/* Macros */
#define OVERFLOW_16BIT  65536    
#define ABS_VAL(x) (x > 0 ? x : -x)

/* Local Function Prototypes */
void EXTI15_10_IRQHandler(void);


TIM_HandleTypeDef htim1;  /* Shared struct with main.c */
Encoder_valTypeDef Encoder_val; /* Local struct */

/* Global variables */
int32_t  enc_increment = 0; 
uint16_t cnt1          = 0, cnt2 = 0;
uint32_t enc_init_time = 0;

Encoder_valTypeDef Enc_GetIncrement(uint8_t click_val)
{

  cnt2 = __HAL_TIM_GET_COUNTER(&htim1);

  if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1)) {
    enc_increment = ABS_VAL(cnt2 - cnt1);

    if (enc_increment> 3000)  {
      enc_increment -= OVERFLOW_16BIT;
    }
    else {
      enc_increment = cnt2 - cnt1;
    }
    cnt1 = cnt2;
  } else if  (!__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1)) {
    enc_increment = ABS_VAL(cnt2 - cnt1);
    if (enc_increment> 3000)  {
      enc_increment = OVERFLOW_16BIT - enc_increment;
    }
    else {
      enc_increment = cnt2 - cnt1;
    }
    cnt1 = cnt2;
  }
  Encoder_val.increment = (int8_t)enc_increment;
  if (click_val != 0) {
    Encoder_val.click = click_val;
  }
  else {
    Encoder_val.click = 0;
  }

  return Encoder_val;
}

void Enc_Init(void) 
{
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  cnt1 = __HAL_TIM_GET_COUNTER(&htim1);
  enc_init_time = HAL_GetTick();
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void) {
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
  Enc_GetIncrement(1);
}
