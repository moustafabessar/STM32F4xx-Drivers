/*
 * main.c
 *
 *  Created on: Jun 11, 2020
 *      Author: Mostafa Besar
 */

#include "stm32f446xx.h"

int main()
{
	return 0;
}

void EXTI0_IRQHandler(void)
{
	// handle the interrupt
    GPIO_IRQHandling(0);
}
