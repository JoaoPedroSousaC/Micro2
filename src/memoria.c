#include "stm32f4xx.h"
#include "Utility.h"
#include "W25Q16.h"

void readMutiples(int initial, int total)
{
	printf("Frase: ");
	for (int i = total; i >= initial; i--){
		printf("%c", Read_Data(i));
	}
	printf("\n");
}

void writeMutiples(char *word, int firstSlot)
{
	int totalSlots = strlen(word);
	int lastSlot = firstSlot + totalSlots;
	int charIndex = 0;


	for (int i = firstSlot; i <= lastSlot; i++)
	{
		Page_Program(i, word[charIndex]);
		charIndex += 1;
	}
}

int checkPercent(int index, int total, int lastPercent)
{

	int perc = ((100 * index) / total) * 10;

	if(lastPercent == perc){
		return perc;
	}


//	printf("%d %%\n", perc / 10, index);


	return perc;
}

void findTeacherPhrase()
{
	int totalSlots = 8192 * 256;

	int posInitial = 0;
	int posFinal = 0;
	int findInitial = 0;
	int lastPercent = -1;
//	printf("procurando");

	for (int i = 0; i<= totalSlots; i+= 1){

		lastPercent = checkPercent(i, totalSlots, lastPercent);

		if (Read_Data(i) != 0xFF && findInitial == 0)
		{
			posInitial = i;
			findInitial = 1;
		}

		if(Read_Data(i) == 0xFF && findInitial == 1){
			posFinal = i -1;
			break;
		}
	}

	int paginaInicial = posInitial / 256;
	int paginaFinal = posFinal / 256;

	readMutiples(posInitial, posFinal);


		printf("Endereco: 0x%x \n", posInitial);



}

int main(void)
{
    Configure_Clock();
    USART1_Init();
    SPI1_W25Q16_Init();
    printf("procurando\n");
    findTeacherPhrase();

}
