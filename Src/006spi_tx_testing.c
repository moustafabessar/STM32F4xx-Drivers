/*
 * 006spi_tx_testing.c
 *
 *  Created on: Jun 27, 2020
 *      Author: Mostafa Besar
 */

/*
 * PB14   =>  MISO
 * PB15   =>  MOSI
 * PB13   =>  SCLK
 * PB12   =>  NSS
 * ALT function Mode
 */
#include "stm32f446xx.h"
#include<string.h>

void SPI2_GPIOInits(void)
{
	 GPIO_Handle_t SPIPins;

	 SPIPins.pGPIOx = GPIOB;
	 SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	 SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	 SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_TYPE_PP;
	 SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	 SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	 //SCLK
	 SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	 GPIO_Init(&SPIPins);

	 //MOSI
	 SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	 GPIO_Init(&SPIPins);

	 //MISO
	 //SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	 //GPIO_Init(&SPIPins);

	 //NSS
	 //SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	 //GPIO_Init(&SPIPins);
}

 void SPI2_Inits(void)
{
	 SPI_Handle_t SPI2handle;
	 SPI2handle.pSPIx = SPI2;
	 SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	 SPI2handle.SPIConfig.SPI_DeviceMode = SPI_Device_MODE_MASTER;
	 SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2; //generate SCLK of 8MHz
	 SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	 SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	 SPI2handle.SPIConfig.SPI_DFF = SPI_DDF_8BITS;
	 SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN;                //software slave managment enabled for NSS pin

	 SPI_Init(&SPI2handle);

}

int main(void)
{
	char user_data[] = "Hello world";
	// this function is used to intialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();
	SPI2_Inits();

    //this NSS signal internally high and avoids MODF error
	SPI_SSIConfig(SPI2, ENABLE);

	//enable the SPI2 peripheral
	SPI_PeriphralControl(SPI2, ENABLE);

	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

	while(1);
	return 0;
}

