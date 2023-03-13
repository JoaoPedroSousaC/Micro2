/* Descriï¿½ï¿½o:
 Implementaï¿½ï¿½o de uma comunicaï¿½ï¿½o serial simples controlada por software:
 - O pino PA0 opera em dreno aberto como pino de comunicaï¿½ï¿½o entre duas placas
 - Os pinos PA0 de duas placas devem ser interligados juntamente com o GND
 - Os botï¿½es K0 e K1 acionam, respectivamente, os LEDs D2 e D3 da outra placa (operaï¿½ï¿½o remota)
 - Hï¿½ um buzzer no pino PA1 para sinalizar o envio de um dado
 - Velocidade de comunicaï¿½ï¿½o de 100 kbps
*/

#include "stm32f4xx.h"
#include "Utility.h"

//Protï¿½tipos de funï¿½ï¿½es
void envia_cmd(uint8_t);	//funï¿½ï¿½o para enviar um comando no barramento
uint8_t recebe_cmd(void);	//funï¿½ï¿½o para receber um comando
void buzzer(void);			//funï¿½ï¿½o de ativaï¿½ï¿½o do buzzer

int main(void)
{
	Utility_Init();	//inicializa funï¿½ï¿½es ï¿½teis

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN;	//habilita o clock do GPIOA e GPIOE

	GPIOA->ODR |= (1<<7) | (1<<6) | (1<<5) | (1 << 2) ;		//inicia com leds e buzzer desligados e linha COM em idle
	GPIOA->OTYPER |= (1 << 2);						//saï¿½da open-drain em PA2
	GPIOA->PUPDR |= (0b01 << 4);					//habilita pull-up em PA2
	GPIOA->MODER |= (0b01 << 14) | (0b01 << 12) | (0b01 << 10) | (0b01 << 2) | (0b01 << 4) ; 	//pinos PA2, PA1, PA6 e PA7 pa5 no modo saï¿½da
	GPIOE->PUPDR |= (0b01 << 8)  | (0b01 << 6);								//habilita pull-up em PE4 e PE3
	GPIOA->PUPDR |= (0b10);
	Delay_ms(100);	//aguarda sinais estabilizarem

	while(1)	//loop infinito
	{
		if(!(GPIOE->IDR & (1 << 4)))	//verifica se PE4 foi pressionado
		{
			envia_cmd(0);						//envia o valor 0
			buzzer();							//sinaliza o fim do envio
			Delay_ms(75);						//filtro de bouncing da chave
			while(!(GPIOE->IDR & (1 << 4)));	//aguarda o botï¿½o ser solto
		}

		if(!(GPIOE->IDR & (1 << 3)))			//verifica se PE3 foi pressionado
		{
			envia_cmd(1);						//envia o valor 1
			buzzer();							//sinaliza o fim do envio
			Delay_ms(75);						//filtro de bouncing da chave
			while(!(GPIOE->IDR & (1 << 3)));	//aguarda o botï¿½o ser solto
		}
		if((GPIOA->IDR & 1))			//verifica se Pa0 foi pressionado
		{
			envia_cmd(2);						//envia o valor 2
			buzzer();							//sinaliza o fim do envio
			Delay_ms(75);						//filtro de bouncing da chave
			while((GPIOA->IDR & 1));	//aguarda o botï¿½o ser solto
		}

		if(!(GPIOA->IDR & (1 << 2)))	//verifica se houve start bit comunicaï¿½ï¿½o
		{
			uint8_t recebido = recebe_cmd();	//recebe o comando
			if(recebido == 0)
			{
				GPIOA->ODR ^= 1 << 6;			//alterna o estado do LED em PA6
			}
			if(recebido == 1)
			{
				GPIOA->ODR ^= 1 << 7;			//alterna o estado do LED em PA7
			}
			if(recebido == 2)
			{
				GPIOA->ODR ^= 1 << 5;			//alterna o estado do LED em PA5
			}
		}
	}
}

//Funï¿½ï¿½o para envio de um comando
void envia_cmd(uint8_t dado)
{
	GPIOA->ODR &= ~(1<<2);	//start bit
	Delay_us(10);		//aguarda tempo do start bit
	if(dado & 1)		//envia o bit do comando
		GPIOA->ODR &= ~(1<<2);
	else if (dado & 2)
		GPIOA->ODR |= (1<<2);
	else
		GPIOA->ODR &= ~(1<<2);
	Delay_us(10);			//aguarda o tempo do bit

	if(dado & 1)		//envia o bit do comando
		GPIOA->ODR |= (1<<2);
	else if (dado & 2)
		GPIOA->ODR &= ~(1<<2);
	else
		GPIOA->ODR &= ~(1<<2);
	Delay_us(10);			//aguarda o tempo do bit
	GPIOA->ODR |= (1<<2);		//stop bit
	Delay_us(5);
	if((GPIOA->IDR & (1<<2)))	//verifica se Ã± houve start bit comunicaï¿½ï¿½o
	{
		if (dado & 1)
		{
			GPIOA->ODR ^= 1 << 7;
		}
		else if (dado & 2)
		{
			GPIOA->ODR ^= 1 << 5;
		}
		else
		{
			GPIOA->ODR ^= 1 << 6;
		}

	}
	Delay_us(5);
	GPIOA->ODR |= (1<<2);
	Delay_us(10);
}

//Funï¿½ï¿½o para recebimento de um comando
uint8_t recebe_cmd(void)
{
	uint8_t dado_recebido;
	uint8_t dado_recebido2;
	Delay_us(5);			//aguarda metade do start bit
	if(!(GPIOA->IDR & (1<<2)))	//confirma que houve um start bit
	{
		Delay_us(10);		//aguarda o tempo do bit
		if(GPIOA->IDR & (1<<2))
			dado_recebido = 1;
		else
			dado_recebido = 0;
		Delay_us(10);		//aguarda o tempo do bit
		if(GPIOA->IDR & (1<<2))
			dado_recebido2 = 1;
		else
			dado_recebido2 = 0;
		Delay_us(5);
		GPIOA->ODR &= ~(1<<2);
		Delay_us(10);			//aguarda para fazer leitura do stop bit
		GPIOA->ODR |= (1<<2);

		Delay_us(10);
		if((GPIOA->IDR & (1<<2)))	//confirma que houve um stop bit
		{
			Delay_us(5);			//aguarda o fim do tempo do stop bit
			if(dado_recebido) {
				dado_recebido = 0b10;
				return dado_recebido;	//retorna o dado recebido
			} else {
				if (dado_recebido2) {
					dado_recebido = 0b01;
				} else {
					dado_recebido = 0b00;
				}
				return dado_recebido;	//retorna o dado recebido
			}

		}
		else
			return 255;	//nï¿½o houve recepï¿½ï¿½o do stop bit, aborta recepï¿½ï¿½o
	}
	else
		return 255;		//nï¿½o houve recepï¿½ï¿½o do start bit, aborta recepï¿½ï¿½o
}

//Funï¿½ï¿½o de sinalizaï¿½ï¿½o de fim de envio de dado
void buzzer(void)
{
	GPIOA->ODR |= 1 << 1;			//liga o buzzer
	Delay_ms(10);					//aguarda
	GPIOA->ODR &= ~(1 << 1);		//desliga o buzzer
	Delay_ms(80);					//aguarda
	GPIOA->ODR |= 1 << 1;			//liga o buzzer
	Delay_ms(10);					//aguarda
	GPIOA->ODR &= ~(1 << 1);		//desliga o buzzer
}
