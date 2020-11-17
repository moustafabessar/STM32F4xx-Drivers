/*
 * 007spi_txonly_arduino.c
 *
 *  Created on: Jul 1, 2020
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


void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i++);
}

void SPI2_GPIOInits(void)
{
	 GPIO_Handle_t SPIPins;

	 SPIPins.pGPIOx = GPIOB;
	 SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	 SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;          //AF5
	 SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_TYPE_PP;
	 SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;
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
	 SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	 GPIO_Init(&SPIPins);
}

 void SPI2_Inits(void)
{
	 SPI_Handle_t SPI2handle;
	 SPI2handle.pSPIx = SPI2;
	 SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	 SPI2handle.SPIConfig.SPI_DeviceMode = SPI_Device_MODE_MASTER;
	 SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8; //generate SCLK of 2MHz  =>  16/2
	 SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	 SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	 SPI2handle.SPIConfig.SPI_DFF = SPI_DDF_8BITS;
	 SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI;                //hardware slave managment enabled for NSS pin

	 SPI_Init(&SPI2handle);

}

 void GPIO_Button_Init(void)
 {
	    GPIO_Handle_t GpioBtn;
		GpioBtn.pGPIOx = GPIOC;
		GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
		GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
		GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
		GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

		//GPIO_PeriClockControl(GPIOC, ENABLE);
		GPIO_Init(&GpioBtn);
 }
int main(void)
{

	GPIO_Button_Init();

	char user_data[] = "An Arduino Uno board is best suited for beginners who have just started using microcontrollers, on the other hand, Arduino Mega board is for enthusiasts who require a lot of I/O pins for their projects";
	// this function is used to intialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();

	// this function is used to intialize the SPI2 peripheral parameters
	SPI2_Inits();

	/*
	 * making SSOE 1 does NSS output enable.
	 * the NSS pin is automatically managed by the hardware.
	 * i.e when SPE=1 , NSS will pulled to low.
	 * and NSS pin will be high when SPE=0
	 */
    SPI_SSOEConfig(SPI2, ENABLE);

    while(1)
    {


	while( GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
    delay();
	//enable the SPI2 peripheral SPE =1
	SPI_PeriphralControl(SPI2, ENABLE);

	//first send length information
	uint8_t dataLen = strlen(user_data);
	SPI_SendData(SPI2, &dataLen, 1);    // 1 means one byte of data

	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

	//lets confirm SPI is not busy
	while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

	//disable the SPI2 peripheral SPE = 0
	SPI_PeriphralControl(SPI2, DISABLE);
    }

	return 0;
}


