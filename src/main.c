/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"

void delay( int n) {
	while (n > 0) {
		n--;
	}
}

int main(void) {
	RCC -> AHB1ENR |= 1;
	GPIOA -> MODER |= 1 <<12;

	while(1) {
		GPIOA -> ODR |= 1 << 6;
		delay(1000000);
		GPIOA -> ODR &= ~(1 << 6);
		delay(4000000);
	}
}
