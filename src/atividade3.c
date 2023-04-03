/* Descrição:
 Implementação de uma comunicação serial simples controlada por software:
 - O pino PA0 opera em dreno aberto como pino de comunicação entre duas placas
 - Os pinos PA0 de duas placas devem ser interligados juntamente com o GND
 - Os botões K0 e K1 acionam, respectivamente, os LEDs D2 e D3 da outra placa (operação remota)
 - Há um buzzer no pino PA1 para sinalizar o envio de um dado
 - Velocidade de comunicação de 100 kbps
*/

#include "stm32f4xx.h"
#include "stdio.h"
#include "Utility.h"

int flag = -1;

void alternarLed1(){
	GPIOA->ODR ^= 1 << 6;			//alterna o estado do LED em PA6
}

void alternarLed2(){
	GPIOA->ODR ^= 1 << 7;
}

int botao1(){
	return !(GPIOE->IDR & (1 << 3));
}

int botao2(){
	return !(GPIOE->IDR & (1 << 4));
}

enviaCMD(int dado){
	USART1->DR = dado;
	flag = 0;
	Delay_ms(100);
	if(flag == 0){
		if(dado == 0){
			alternarLed1();
		}else if(dado == 1){
			alternarLed2();
		}
	}

}

void USART1_IRQHandler(void)
	{
		int recebido = USART1->DR;

		if(recebido == 0){
			alternarLed1();
			USART1->DR = 2;
		}
		if(recebido == 1){
			alternarLed2();
			USART1->DR = 2;
		}
		if (recebido == 2) {
			flag = -1;
		}

	}

int main(void)
{
	Utility_Init();	//inicializa funções úteis
	USART1_Init();


	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN;	//habilita o clock do GPIOA e GPIOE

	GPIOA->ODR |= (1<<7) | (1<<6);		//inicia com leds e buzzer desligados e linha COM em idle
	GPIOA->MODER |= (0b01 << 14) | (0b01 << 12);	//pinos PA7  PA6, PA1, PA2 e PA8 no modo saída
	GPIOE->PUPDR |= (0b01 << 8) | (0b01 << 6);					//habilita pull-up em PE4 e PE3

	Delay_ms(100);	//aguarda sinais estabilizarem


	while(1)	//loop infinito
		{

			if(botao1())
			{
				flag = 0;
				enviaCMD(0);
				Delay_ms(75);
				while(botao1());
			}

			if(botao2())
			{
				flag = 0;
				enviaCMD(1);
				Delay_ms(75);
				while(botao2());
			}

		}
}
