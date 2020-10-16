/* Includes */
#include "encoder.h"
#include "main.h"

/* Macros */
#define OVERFLOW_16BIT  65536    

/* Local Function Prototypes */
void EXTI15_10_IRQHandler(void);

TIM_HandleTypeDef htim1; /* Shared struct with main.c */

/* Read
 https://stackoverflow.com/questions/3041797/how-to-use-a-defined-struct-from-another-source-file
 /*/

/* Global variables */
uint8_t click;
uint16_t cnt1 = 0;

int8_t Enc_GetIncrement(void) {
	int32_t enc_increment = 0;
	uint16_t cnt2 = 0;
	cnt2 = __HAL_TIM_GET_COUNTER(&htim1);

	if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1)) {
		enc_increment = ABS_VAL(cnt2 - cnt1);

		if (enc_increment > 3000) {
			enc_increment -= OVERFLOW_16BIT;
		} else {
			enc_increment = cnt2 - cnt1;
		}
		cnt1 = cnt2;
	} else if (!__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1)) {
		enc_increment = ABS_VAL(cnt2 - cnt1);
		if (enc_increment > 3000) {
			enc_increment = OVERFLOW_16BIT - enc_increment;
		} else {
			enc_increment = cnt2 - cnt1;
		}
		cnt1 = cnt2;
	}
	return (int8_t)enc_increment;
}

uint8_t Enc_getClick(void)
{
	return click;
}

void Enc_Init(void) {
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	cnt1 = __HAL_TIM_GET_COUNTER(&htim1);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void) {
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
	click = !click;
}
