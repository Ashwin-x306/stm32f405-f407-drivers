/*
 * stm32f405xxx_usart_driver.c
 *
 *  Created on: Aug 10, 2026
 *      Author: Ashwin
 */
#include"stm32f405xxx_usart_driver.h"

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pUSARTx == USART1)    RCC->APB2ENR |= (1 << 4);

		else if(pUSARTx == USART2){
			RCC->APB1ENR |= (1 << 17);
		}
		else if(pUSARTx == USART3)
		{
			RCC->APB1ENR |= (1 << 18);
		}
		else if(pUSARTx == UART4)
		{
			RCC->APB1ENR |= (1 << 19);
		}
		else if(pUSARTx == UART5)
		{
			RCC->APB1ENR |= (1 << 20);
		}
		else if(pUSARTx == USART6)
		{
			RCC->APB2ENR |= (1 << 5);
		}
	}
	else
	{
		if(pUSARTx == USART1)
		{
			RCC->APB2ENR &= ~(1 << 4);
		}
		else if(pUSARTx == USART2)
		{
			RCC->APB1ENR &= ~(1 << 17);
		}
		else if(pUSARTx == USART3)
		{
			RCC->APB1ENR &= ~(1 << 18);
		}
		else if(pUSARTx == UART4)
		{
			RCC->APB1ENR &= ~(1 << 19);
		}
		else if(pUSARTx == UART5)
		{
			RCC->APB1ENR &= ~(1 << 20);
		}
		else if(pUSARTx == USART6)
		{
			RCC->APB2ENR &= ~(1 << 5);
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_SetBaudRate
 *
 * @brief             - Programs the USART_BRR register for the requested
 *                       baud rate, based on peripheral clock (PCLK1/PCLK2)
 *
 * @param[in]         - Base address of the USART peripheral
 * @param[in]         - Desired baud rate
 *
 * @return            - none
 *
 * @Note              - Uses RCC_GetPCLK1Value()/RCC_GetPCLK2Value() which
 *                       must be provided by your RCC driver.
 *********************************************************************/
void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
	uint32_t PCLKx;
	uint32_t usartdiv;
	uint32_t M_part, F_part;
	uint32_t tempreg = 0;

	if(pUSARTx == USART1 || pUSARTx == USART6)
	{
		PCLKx = RCC_GetPCLK2Value();
	}
	else
	{
		PCLKx = RCC_GetPCLK1Value();
	}

	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	{
		/* OVER8 = 1 , over sampling by 8 */
		usartdiv = ((25 * PCLKx) / (2 * BaudRate));
	}
	else
	{
		/* OVER8 = 0 , over sampling by 16 */
		usartdiv = ((25 * PCLKx) / (4 * BaudRate));
	}

	M_part = usartdiv / 100;
	tempreg |= (M_part << 4);

	F_part = usartdiv - (M_part * 100);

	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	{
		F_part = (((F_part * 8) + 50) / 100) & ((uint8_t)0x07);
	}
	else
	{
		F_part = (((F_part * 16) + 50) / 100) & ((uint8_t)0x0F);
	}

	tempreg |= F_part;

	pUSARTx->BRR = tempreg;
}

/*********************************************************************
 * @fn      		  - USART_Init
 *
 * @brief             - Initializes the USART peripheral according to
 *                       the settings in pUSARTHandle->USART_Config
 *
 * @param[in]         - pointer to USART handle structure
 *
 * @return            - none
 *
 * @Note              - none
 *********************************************************************/
void USART_Init(USART_Handle_t *pUSARTHandle)
{
	uint32_t tempreg = 0;

	/* Enable the peripheral clock */
	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

	/************************** CR1 configuration ******************************/

	if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		tempreg |= (1 << USART_CR1_RE);
	}
	else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		tempreg |= (1 << USART_CR1_TE);
	}
	else if(pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		tempreg |= ((1 << USART_CR1_TE) | (1 << USART_CR1_RE));
	}

	/* Word length */
	tempreg |= pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M;

	/* Parity control */
	if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		tempreg |= (1 << USART_CR1_PCE);
		/* PS = 0 for even parity (default), nothing else to set */
	}
	else if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
	{
		tempreg |= (1 << USART_CR1_PCE);
		tempreg |= (1 << USART_CR1_PS);
	}

	pUSARTHandle->pUSARTx->CR1 = tempreg;

	/************************** CR2 configuration ******************************/

	tempreg = 0;

	tempreg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;

	pUSARTHandle->pUSARTx->CR2 = tempreg;

	/************************** CR3 configuration ******************************/

	tempreg = 0;

	if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		tempreg |= (1 << USART_CR3_CTSE);
	}
	else if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		tempreg |= (1 << USART_CR3_RTSE);
	}
	else if(pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		tempreg |= (1 << USART_CR3_CTSE);
		tempreg |= (1 << USART_CR3_RTSE);
	}

	pUSARTHandle->pUSARTx->CR3 = tempreg;

	/************************** BRR (Baud rate) configuration **************************/

	USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config.USART_Baud);
}

/*********************************************************************
 * @fn      		  - USART_DeInit
 *
 * @brief             - Resets the given USART peripheral via RCC reset reg
 *********************************************************************/
void USART_DeInit(USART_RegDef_t *pUSARTx)
{
	if(pUSARTx == USART1)
	{
		RCC->APB2RSTR |= (1 << 4);
		RCC->APB2RSTR &= ~(1 << 4);
	}
	else if(pUSARTx == USART2)
	{
		RCC->APB1RSTR |= (1 << 17);
		RCC->APB1RSTR &= ~(1 << 17);
	}
	else if(pUSARTx == USART3)
	{
		RCC->APB1RSTR |= (1 << 18);
		RCC->APB1RSTR &= ~(1 << 18);
	}
	else if(pUSARTx == UART4)
	{
		RCC->APB1RSTR |= (1 << 19);
		RCC->APB1RSTR &= ~(1 << 19);
	}
	else if(pUSARTx == UART5)
	{
		RCC->APB1RSTR |= (1 << 20);
		RCC->APB1RSTR &= ~(1 << 20);
	}
	else if(pUSARTx == USART6)
	{
		RCC->APB2RSTR |= (1 << 5);
		RCC->APB2RSTR &= ~(1 << 5);
	}
}

/*********************************************************************
 * @fn      		  - USART_SendData
 *
 * @brief             - Blocking (polling) transmit
 *********************************************************************/
void USART_SendData(USART_RegDef_t *pUSARTx, uint8_t *pTxBuffer, uint32_t Len)
{
	uint16_t *pdata;

	for(uint32_t i = 0 ; i < Len; i++)
	{
		/* Wait until TXE flag is set (Tx buffer empty) */
		while(!USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE));

		if(pUSARTx->CR1 & (1 << USART_CR1_M))
		{
			/* 9-bit word length */
			pdata = (uint16_t*)pTxBuffer;
			pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

			if(pUSARTx->CR1 & (1 << USART_CR1_PCE))
			{
				/* parity enabled, 8 data bits + 1 parity, user gives 1 byte */
				pTxBuffer++;
			}
			else
			{
				/* no parity, 9 data bits, user gives 2 bytes */
				pTxBuffer++;
				pTxBuffer++;
			}
		}
		else
		{
			/* 8-bit word length */
			pUSARTx->DR = (*pTxBuffer & (uint8_t)0xFF);
			pTxBuffer++;
		}
	}

	/* Wait until TC flag is set (transmission complete) */
	while(!USART_GetFlagStatus(pUSARTx, USART_FLAG_TC));
}

/*********************************************************************
 * @fn      		  - USART_ReceiveData
 *
 * @brief             - Blocking (polling) receive
 *********************************************************************/
void USART_ReceiveData(USART_RegDef_t *pUSARTx, uint8_t *pRxBuffer, uint32_t Len)
{
	for(uint32_t i = 0 ; i < Len; i++)
	{
		/* Wait until RXNE flag is set (data available) */
		while(!USART_GetFlagStatus(pUSARTx, USART_FLAG_RXNE));

		if(pUSARTx->CR1 & (1 << USART_CR1_M))
		{
			/* 9-bit word length */
			if(pUSARTx->CR1 & (1 << USART_CR1_PCE))
			{
				/* parity enabled, only 8 useful data bits */
				*pRxBuffer = (uint8_t)(pUSARTx->DR & (uint8_t)0xFF);
				pRxBuffer++;
			}
			else
			{
				/* no parity, all 9 bits are data */
				*((uint16_t*)pRxBuffer) = (uint16_t)(pUSARTx->DR & (uint16_t)0x01FF);
				pRxBuffer++;
				pRxBuffer++;
			}
		}
		else
		{
			/* 8-bit word length */
			if(pUSARTx->CR1 & (1 << USART_CR1_PCE))
			{
				/* parity enabled, only 7 useful data bits */
				*pRxBuffer = (uint8_t)(pUSARTx->DR & (uint8_t)0x7F);
			}
			else
			{
				/* no parity, all 8 bits are data */
				*pRxBuffer = (uint8_t)(pUSARTx->DR & (uint8_t)0xFF);
			}
			pRxBuffer++;
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_SendDataIT
 *
 * @brief             - Interrupt based transmit. Enables TXEIE and TCIE
 *                       and lets the ISR move the data.
 *********************************************************************/
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t txstate = pUSARTHandle->TxBusyState;

	if(txstate != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = Len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
	}

	return txstate;
}

/*********************************************************************
 * @fn      		  - USART_ReceiveDataIT
 *
 * @brief             - Interrupt based receive. Enables RXNEIE and lets
 *                       the ISR move the data.
 *********************************************************************/
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t rxstate = pUSARTHandle->RxBusyState;

	if(rxstate != USART_BUSY_IN_RX)
	{
		pUSARTHandle->RxLen = Len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		(void)pUSARTHandle->pUSARTx->DR; /* dummy read to clear stale RXNE, optional */

		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
	}

	return rxstate;
}

/*********************************************************************
 * @fn      		  - USART_IRQInterruptConfig
 *
 * @brief             - Enables/disables the given IRQ number at the
 *                       NVIC ISER/ICER registers
 *********************************************************************/
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_IRQPriorityConfig
 *
 * @brief             - Sets the priority of the given IRQ number
 *********************************************************************/
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;
	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/*********************************************************************
 * @fn      		  - USART_IRQHandling
 *
 * @brief             - Interrupt handler for USART. Call this from the
 *                       corresponding USARTx_IRQHandler().
 *********************************************************************/
void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{
	uint32_t temp1, temp2, temp3;
	uint16_t *pdata;

	/*************************** TC (Transmission Complete) *******************/
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TC);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TCIE);

	if(temp1 && temp2)
	{
		/* only close Tx if all bytes were transmitted */
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			if(!pUSARTHandle->TxLen)
			{
				/* clear TC flag */
				pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_TC);
				/* clear TCIE */
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE);

				pUSARTHandle->TxBusyState = USART_READY;
				pUSARTHandle->pTxBuffer = NULL;
				pUSARTHandle->TxLen = 0;

				USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_TX_CMPLT);
			}
		}
	}

	/*************************** TXE (Tx buffer empty) *******************/
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_TXE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TXEIE);

	if(temp1 && temp2)
	{
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
		{
			if(pUSARTHandle->TxLen > 0)
			{
				if(pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_M))
				{
					/* 9-bit */
					pdata = (uint16_t*)pUSARTHandle->pTxBuffer;
					pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

					if(pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_PCE))
					{
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen--;
					}
					else
					{
						pUSARTHandle->pTxBuffer += 2;
						pUSARTHandle->TxLen -= 2;
					}
				}
				else
				{
					/* 8-bit */
					pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer & (uint8_t)0xFF);
					pUSARTHandle->pTxBuffer++;
					pUSARTHandle->TxLen--;
				}
			}

			if(pUSARTHandle->TxLen == 0)
			{
				/* disable TXEIE, TC interrupt (TCIE) is still enabled
				 * to signal completion once last byte shifts out */
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TXEIE);
			}
		}
	}

	/*************************** RXNE (Received data ready) *******************/
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_RXNE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);

	if(temp1 && temp2)
	{
		if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
		{
			if(pUSARTHandle->RxLen > 0)
			{
				if(pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_M))
				{
					/* 9-bit */
					if(pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_PCE))
					{
						*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen--;
					}
					else
					{
						*((uint16_t*)pUSARTHandle->pRxBuffer) = (uint16_t)(pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF);
						pUSARTHandle->pRxBuffer += 2;
						pUSARTHandle->RxLen -= 2;
					}
				}
				else
				{
					/* 8-bit */
					if(pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_PCE))
					{
						*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
					}
					else
					{
						*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
					}
					pUSARTHandle->pRxBuffer++;
					pUSARTHandle->RxLen--;
				}
			}

			if(!pUSARTHandle->RxLen)
			{
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_RXNEIE);
				pUSARTHandle->RxBusyState = USART_READY;
				USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_RX_CMPLT);
			}
		}
	}

	/*************************** CTS *******************/
	/* CTS flag exists only for USART1, USART2, USART3 (not UART4/5) */
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_CTS);
	temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSE);
	temp3 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_CTSIE);

	if(temp1 && temp2 && temp3)
	{
		pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_CTS);
		USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_CTS);
	}

	/*************************** IDLE *******************/
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_IDLE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_IDLEIE);

	if(temp1 && temp2)
	{
		/* clear IDLE flag: read SR then read DR */
		temp1 = pUSARTHandle->pUSARTx->SR;
		temp1 = pUSARTHandle->pUSARTx->DR;
		(void)temp1;
		USART_ApplicationEventCallback(pUSARTHandle, USART_EVENT_IDLE);
	}

	/*************************** Overrun error *******************/
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_ORE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);
	temp3 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_EIE);

	if(temp1 && temp2 && temp3)
	{
		/* not clearing ORE here on purpose; let the app decide
		 * (clear via USART_ClearFlag or by reading SR then DR) */
		USART_ApplicationEventCallback(pUSARTHandle, USART_ERR_ORE);
	}

	/*************************** Error interrupt (FE, NE, ORE via EIE) *******************/
	temp2 = pUSARTHandle->pUSARTx->CR3 & (1 << USART_CR3_EIE);

	if(temp2)
	{
		temp1 = pUSARTHandle->pUSARTx->SR;

		if(temp1 & (1 << USART_SR_FE))
		{
			USART_ApplicationEventCallback(pUSARTHandle, USART_ERR_FE);
		}
		if(temp1 & (1 << USART_SR_NE))
		{
			USART_ApplicationEventCallback(pUSARTHandle, USART_ERR_NE);
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_PeripheralControl
 *
 * @brief             - Enables/disables the USART peripheral (UE bit)
 *********************************************************************/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pUSARTx->CR1 |= (1 << USART_CR1_UE);
	}
	else
	{
		pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
	}
}

/*********************************************************************
 * @fn      		  - USART_GetFlagStatus
 *
 * @brief             - Returns SET/RESET for the given flag in SR
 *********************************************************************/
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName)
{
	if(pUSARTx->SR & FlagName)
	{
		return SET;
	}
	return RESET;
}

/*********************************************************************
 * @fn      		  - USART_ClearFlag
 *
 * @brief             - Clears the given status flag in SR (write 0)
 *********************************************************************/
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName)
{
	pUSARTx->SR &= ~(StatusFlagName);
}

/*********************************************************************
 * @fn      		  - USART_ApplicationEventCallback
 *
 * @brief             - Weak callback; the application should override
 *                       this to handle Tx/Rx complete and error events.
 *********************************************************************/
__attribute__((weak)) void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t AppEv)
{
	/* This is a weak implementation. The application may override this
	 * function by defining the same signature in the app code. */
}
