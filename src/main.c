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
	RCC -> AHB1ENR |= 1;//liga clock a
	RCC -> AHB1ENR |= 1 << 4; //liga clock e

	GPIOA -> MODER |= 1 <<12;//coloca como saida pa6
	GPIOA -> MODER |= 1 <<14;//coloca como saida pa7

	GPIOE->MODER &= ~(0b11 << 8); //seleciona modo de entrada digital no pino PE4
	GPIOE->PUPDR |= (0b01 << 8); //habilita o resistor de pull-down no pino PE4
	GPIOE->MODER &= ~(0b11 << 6); //seleciona modo de entrada digital no pino PE3
	GPIOE->PUPDR |= (0b01 << 6); //habilita o resistor de pull-down no pino PE3

	int flagK0 = 0;
	int flagK1 = 0;

	GPIOA -> ODR |= (1 << 6) | (1 << 7);//aciona os dois leds em uma mesma linha

	while(1) {
		if(!(GPIOE->IDR  & (1 << 4)) ) {//verifica se foi acionado o botão pe4
			if (flagK0) {
				GPIOA -> ODR &= ~(1 << 6);
				flagK0 = 0;
			} else {
				GPIOA -> ODR |= 1 << 6;
				flagK0 = 1;
			}
			delay(1000000);
		}
		if(!(GPIOE->IDR  & (1 << 3))) {//verifica se foi acionado o botão pe3
			if (flagK1) {
				GPIOA -> ODR &= ~(1 << 7);
				flagK1 = 0;
			} else {
				GPIOA -> ODR |= 1 << 7;
				flagK1 = 1;
			}
			delay(1000000);
		}

	}
}
