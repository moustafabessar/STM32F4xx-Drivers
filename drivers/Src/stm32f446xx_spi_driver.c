/*
 * stm32f446xx_spi_driver.c
 *
 *  Created on: Jun 22, 2020
 *      Author: Mostafa Besar
 */

#include "stm32f446xx_spi_driver.h"


static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	   {
			if(pSPIx == SPI1)
			{
				SPI1_PCLK_EN();
			}
			else if(pSPIx == SPI2)
			{
				SPI2_PCLK_EN();
			}
			else if(pSPIx == SPI3)
			{
				SPI3_PCLK_EN();
			}
			else if(pSPIx == SPI4)
			{
				SPI4_PCLK_EN();
			}

       }else
       {
    	   if(pSPIx == SPI1)
    	   			{
    		            SPI1_PCLK_DIS();
    	   			}
    	   			else if(pSPIx == SPI2)
    	   			{
    	   				SPI2_PCLK_DIS();
    	   			}
    	   			else if(pSPIx == SPI3)
    	   			{
    	   				SPI3_PCLK_DIS();
    	   			}
    	   			else if(pSPIx == SPI4)
    	   			{
    	   				SPI4_PCLK_DIS();
    	   			}
       }
}


void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// enable peripheral clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//first lets configure the SPI_CR1
	uint32_t tempreg = 0;

	//1. configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2.configure bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//bidi mode should be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		// RXONLY bit must be set
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the spi serial clock speed (baud rate)
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4.  Configure the DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	//5. configure the CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6 . configure the CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;
	pSPIHandle->pSPIx->CR1 = tempreg;

}


void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	}

}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;

	}
	return FLAG_RESET;
}

void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer, uint32_t Len)
{
   while(Len > 0)
   {
	   //while(!(pSPIx->SR & (1<<1)));
	   //1. wait until TXE is set
	   while(SPI_GetFlagStatus(pSPIx,SPI_FLAG_TXE)  == FLAG_RESET);

       //2. check the DFF bit in CR1
	   if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
	   {
		   // 16 bit DFF
		   //1. load the data in to the DR
		   pSPIx->DR = *((uint16_t*)pTxBuffer);
		   Len--;
		   Len--;
		   (uint16_t*)pTxBuffer++;
	   }else
	   {
		   pSPIx->DR = *pTxBuffer;
		   Len--;
		   pTxBuffer++;

	   }
   }

}

/*********************************************************************
 * @fn      		  - SPI_ReceiveData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t *pRxBuffer, uint32_t Len)
{
	   while(Len > 0)
	   {

		   //1. wait until RXE is set
		   while(SPI_GetFlagStatus(pSPIx,SPI_FLAG_RXNE)  == FLAG_RESET);

	       //2. check the DFF bit in CR1
		   if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		   {
			   // 16 bit DFF
			   //1. load the data in from the DR to RXbuffer address

			   *((uint16_t*)pRxBuffer) =pSPIx->DR;
			   Len--;
			   Len--;
			   (uint16_t*)pRxBuffer++;
		   }else
		   {
			   *pRxBuffer = pSPIx->DR;
			   Len--;
			   pRxBuffer++;

		   }
	   }
}

/*********************************************************************
 * @fn      		  - SPI_PeriphralControl
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */

void SPI_PeriphralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}


/*********************************************************************
 * @fn      		  - SPI_SSIConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

/*********************************************************************
 * @fn      		  - SPI_SSOEConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -
 */

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}


void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		{
			if(IRQNumber <= 31)
			{
				// program ISER0 register
				*NVIC_ISER0 |= (1 << IRQNumber);

			}else if(IRQNumber > 31 && IRQNumber < 64) // 32 to 63
			{
				// program ISER1 register
				*NVIC_ISER1 |= (1 << (IRQNumber % 32));
			}
			else if(IRQNumber >= 64 && IRQNumber < 96)
			{
				// program ISER2 register
				*NVIC_ISER2 |= (1 << (IRQNumber % 64));
			}
		}
		else
		{
			if(IRQNumber <= 31)
					{
						//program ICER0 register
						*NVIC_ICER0 |= ( 1 << IRQNumber );
					}else if(IRQNumber > 31 && IRQNumber < 64 )
					{
						//program ICER1 register
						*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
					}
					else if(IRQNumber >= 6 && IRQNumber < 96 )
					{
						//program ICER2 register
						*NVIC_ICER3 |= ( 1 << (IRQNumber % 64) );
					}
				}
}

void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. first lets find out the ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber %4 ;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

	*(  NVIC_PR_BASE_ADDR + iprx ) |=  ( IRQPriority << shift_amount );

}

void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	uint8_t temp1, temp2;
	//first check for TXE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		//handle TXE
		spi_txe_interrupt_handle(pHandle);
	}

	//check for RXNE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		//handle RXNE
		spi_rxne_interrupt_handle(pHandle);
	}

	//handle TXE
	spi_txe_interrupt_handle(pHandle);


	//check for ovr flag
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		//handle RXNE
		spi_ovr_err_interrupt_handle(pHandle);
	}

}

//some helper function implementations
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	 // check the DFF bit in CR1
		   if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
		   {
			   // 16 bit DFF
			   //1. load the data in to the DR
			   pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
			   pSPIHandle->TxLen--;
			   pSPIHandle->TxLen--;
			   (uint16_t*)pSPIHandle->pTxBuffer++;
		   }else
		   {
			   pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
			   pSPIHandle->TxLen--;
			   pSPIHandle->pTxBuffer++;
		   }
		   if(! pSPIHandle->TxLen)
		   {
			   //if txLen = 0 then close tx transmission and inform the application that
			   //TX is over,
			   //this prevents interrupts form TXE flag
			   pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
			   pSPIHandle->pTxBuffer = NULL;
			   pSPIHandle->TxLen = 0;
			   pSPIHandle->TxState = SPI_READY;
		   }

}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	 //check the DFF bit in CR1
		   if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
		   {
			   // 16 bit DFF
			   //1. load the data in to the DR
			   pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pRxBuffer);
			   pSPIHandle->RxLen--;
			   pSPIHandle->RxLen--;
			   (uint16_t*)pSPIHandle->pTxBuffer++;
		   }else
		   {
			   pSPIHandle->pSPIx->DR = *pSPIHandle->pRxBuffer;
			   pSPIHandle->RxLen--;
			   pSPIHandle->pRxBuffer++;
		   }
		   if(! pSPIHandle->RxLen)
		   {
			   //if txLen = 0 then close tx reception and inform the application that
			   //TX is ove  k  \][0987r,
			   //this prevents interrupts form RXNE flag
			   pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
			   pSPIHandle->pRxBuffer = NULL;
			   pSPIHandle->RxLen = 0;
			   pSPIHandle->RxState = SPI_READY;
		   }

}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{

}
