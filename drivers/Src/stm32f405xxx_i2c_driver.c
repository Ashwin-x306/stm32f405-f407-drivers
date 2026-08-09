=/*
 * stm32f405xxx_i2c_driver.h
 *
 *  Created on: Jul 23, 2026
 *      Author: Ashwin
 */

#ifndef SRC_STM32F405XXX_I2C_DRIVER_C_
#define SRC_STM32F405XXX_I2C_DRIVER_C_
#include "stm32f405xxx_i2c_driver.h"


static uint8_t  GetFlagStatus(I2C_Handle_t *pI2CHandle , uint8_t Status_regNumber  , uint16_t flag_name   ){
	uint8_t status =0 ;
	if(Status_regNumber == 1) status = pI2CHandle->pI2Cx->SR1  &(flag_name ) ;

	else status= pI2CHandle->pI2Cx->SR2  &(flag_name ) ;
    return status  ;
}
static void  I2C_ManageACK(I2C_Handle_t *I2C_Handle ,  uint8_t ENorDI  ){
	if(ENorDI ==ENABLE){
      I2C_Handle->pI2Cx->CR1 |= (1<<10) ;  
	}
	else{
     I2C_Handle->pI2Cx->CR1&= ~(1<<10) ;  
	}
}


void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx   , uint8_t EnORDi) {
     if(EnORDi == ENABLE ){

       if(pI2Cx == I2C1)     RCC->APB1ENR|= (1<<21) ;
       else if(pI2Cx == I2C2)RCC->APB1ENR|= (1<<22) ;
       else if(pI2Cx == I2C3)RCC->APB1ENR|= (1<<23) ;
     }
       else{
         if(pI2Cx == I2C1)     RCC->APB1ENR &= ~(1<<21) ;
         else if(pI2Cx == I2C2)RCC->APB1ENR &= ~(1<<22) ;
         else if(pI2Cx == I2C3)RCC->APB1ENR &= ~(1<<23) ;
     }
}
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx  , uint8_t EnorDi) {
	if(EnorDi == ENABLE) pI2Cx->CR1 |= (1<<0) ;
	else pI2Cx->CR1 &= ~(1<<0 ) ;

}
void I2C_DeInit(I2C_RegDef_t *pI2Cx){
	       if(pI2Cx == I2C1)     RCC->APB1RSTR |= (1<<21) ;
	       else if(pI2Cx == I2C2)RCC->APB1RSTR |= (1<<22) ;
	       else if(pI2Cx == I2C3)RCC->APB1RSTR |= (1<<23)  ;

	       if(pI2Cx == I2C1)     RCC->APB1RSTR &= ~(1<<21) ;
	       else if(pI2Cx == I2C2)RCC->APB1RSTR &= ~(1<<22) ;
	       else if(pI2Cx == I2C3)RCC->APB1RSTR &= ~(1<<23) ;

}

static uint32_t RCC_GetPLLOutputClock(void ){

	return 0 ;
}
uint16_t AHB_PreScalar[9] =  {2,4,8,16,32,64,128,256,512} ;
uint16_t APB_PreScalar[4] =  {2,4,8,16} ;
static uint32_t RCC_GetPCLK1Value(void ){

	uint32_t pclk1  ,  System_clock;
	uint8_t clk_src  , temp ,ahbp  ,  apb1p;
	clk_src  =((RCC->CFGR >>2)&(0x03)) ;
	if(clk_src == 0 )       System_clock =16000000 ;
    else if(clk_src == 1)	System_clock = 8000000 ;
    else if(clk_src == 2 )  System_clock =RCC_GetPLLOutputClock() ;
	else{
		 ; //  wont enter in here  
	}


	temp = ((RCC->CFGR >>4)&0xF) ;
	if(temp<8)ahbp = 1;

	else ahbp = AHB_PreScalar[temp - 8] ;

	temp = ((RCC->CFGR>>10)&0x7) ;
    if(temp<4)	apb1p = 1 ;

    else apb1p = APB_PreScalar[temp -4 ] ;
	pclk1 = (System_clock/ahbp)/(apb1p) ;

	return pclk1 ;


}
void I2C_Init(I2C_Handle_t *pI2CHandle ){

	uint32_t tempreg = 0 ;

	tempreg|= (pI2CHandle->I2C_Config.I2C_ACKControl<<10) ;
	pI2CHandle->pI2Cx->CR1 = tempreg;
	tempreg= 0 ;
	tempreg =RCC_GetPCLK1Value()/(1000000U) ;
    pI2CHandle->pI2Cx->CR2 = (tempreg&(0x3F)) ;

    tempreg= 0 ;
    tempreg = pI2CHandle->I2C_Config.I2C_DeviceAddress<<1 ;  // bit 7 -`1 ;
    // add mode is always set to 1 considering 7 bit addresss always ;
    tempreg|= (1<<14) ;
    pI2CHandle->pI2Cx->OAR1 = tempreg;
     tempreg = 0 ;
     uint16_t ccr_value = 0 ;
    if(pI2CHandle->I2C_Config.I2C_SCLSpeed <=I2C_SCL_SPEED_SM){
         //  STANDARD  MODE ;
        ccr_value = RCC_GetPCLK1Value()/(2*pI2CHandle->I2C_Config.I2C_SCLSpeed) ;
    	tempreg |= (ccr_value  & 0XFFF) ;  //  12 bits valid ;
    }
    else{
    	// FAST MODE ;
    	tempreg|= (1<<15) ;
    	tempreg|=(pI2CHandle->I2C_Config.I2C_FMDutyCycle<<14) ;
    	if(pI2CHandle->I2C_Config.I2C_FMDutyCycle ==I2C_FM_DUTY_2){
    		ccr_value = (RCC_GetPCLK1Value()) /(3*pI2CHandle->I2C_Config.I2C_SCLSpeed) ;
           }
    	else{
    		ccr_value = RCC_GetPCLK1Value()/(25*(pI2CHandle->I2C_Config.I2C_SCLSpeed)) ;
    	 }
       	tempreg |= (ccr_value  & 0XFFF) ;  //  12 bits valid ;

    }
    pI2CHandle->pI2Cx->CCR = tempreg;
    // this  is still pending ;
	 tempreg = 0 ;
    if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM){
    	  //  standard mode 
    	tempreg = (RCC_GetPCLK1Value() /1000000U) + 1 ;
    }
    else{
    	 //  fast mode ; 
    	tempreg = ( (RCC_GetPCLK1Value() * 300) /1000000000U ) + 1 ;
    }
    pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F) ;   //  6 bits valid ;
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle , uint8_t *pTxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr) {

	//  generate the start condition;
	pI2CHandle->pI2Cx->CR1 |= (1<<8) ;
    while(!(GetFlagStatus (pI2CHandle  ,  1  , I2C_FLAG_SB))) ;  // wait till it sets;

    SlaveAddr =  SlaveAddr<<1 ;
    SlaveAddr &=~(1) ;//  SEND DATA ;  //  clearing the bit 
    pI2CHandle->pI2Cx->DR = SlaveAddr ;
    while(!(GetFlagStatus (pI2CHandle  ,  1  , I2C_FLAG_ADDR))) ;

    uint32_t dummy_sr = pI2CHandle->pI2Cx->SR1;
    dummy_sr = pI2CHandle->pI2Cx->SR2;
    (void)dummy_sr ;

    while(len>0){

	      while(!(GetFlagStatus(pI2CHandle ,1,I2C_FLAG_TXE))) ;  //  WAIT UNTIL TXE IS FREE ;
	      pI2CHandle->pI2Cx->DR  = *pTxBuffer ;
	      pTxBuffer++ ;
	      len-- ;
    }
    while(!(GetFlagStatus(pI2CHandle ,1, I2C_FLAG_TXE))) ;
    while(!(GetFlagStatus(pI2CHandle, 1,I2C_FLAG_BTF))) ;  
	// stop condition ; 
	pI2CHandle->pI2Cx->CR1 |=(1<<9) ;  

}
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle , uint8_t *pRxBuffer, uint32_t len, uint8_t SlaveAddr){
 
    pI2CHandle->pI2Cx->CR1|= (1<<8) ;  //  to configure the start condition ;  
	while(!(GetFlagStatus(pI2CHandle , 1 ,  I2C_FLAG_SB))) ; 
	SlaveAddr = SlaveAddr<<1 ; 
	SlaveAddr|=1 ;  //  bit  is set   
	pI2CHandle->pI2Cx->DR= SlaveAddr ; 
	if(len == 1 ){

	I2C_ManageACK(pI2CHandle ,  DISABLE ) ; 
	while(!(GetFlagStatus (pI2CHandle  ,  1  , I2C_FLAG_ADDR))) ;

    uint32_t dummy_sr = pI2CHandle->pI2Cx->SR1;
    dummy_sr = pI2CHandle->pI2Cx->SR2;
    (void)dummy_sr ;
	pI2CHandle->pI2Cx->CR1 |=(1<<9) ; 
	while(!(GetFlagStatus(pI2CHandle ,1,I2C_FLAG_RXNE))) ;  //  WAIT UNTIL TXE IS FREE ;

   *pRxBuffer = pI2CHandle->pI2Cx->DR ; 

	 return  ; 
	}
	if(len > 1){ 

		while(!(GetFlagStatus (pI2CHandle  ,  1  , I2C_FLAG_ADDR))) ;

	    uint32_t dummy_sr = pI2CHandle->pI2Cx->SR1;
	    dummy_sr = pI2CHandle->pI2Cx->SR2;
	    (void)dummy_sr ;

		for(uint32_t i = len ; i>0 ; i--){
			while(!(GetFlagStatus(pI2CHandle ,1,I2C_FLAG_RXNE))) ;  //  WAIT UNTIL RXNE IS FREE ;
            if(i ==2 ) {


            	I2C_ManageACK(pI2CHandle , DISABLE) ;
            	pI2CHandle->pI2Cx->CR1 |=(1<<9) ;
             }
			*pRxBuffer = pI2CHandle->pI2Cx->DR ; 
			 pRxBuffer++ ; 
			
        }
	}
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE ){
            I2C_ManageACK(pI2CHandle , DISABLE) ; 
	}



}

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2Cx , uint8_t  *pTxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr , uint8_t Sr) {

	uint8_t busystate = pI2Cx->TxRxState ;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX) ){

		pI2Cx->pTxBuffer = pTxBuffer ;
		pI2Cx->TxLen = len ;
		pI2Cx->TxRxState = I2C_BUSY_IN_TX ;
		pI2Cx->DevAddr = SlaveAddr ;


		pI2Cx->pI2Cx->CR1 |= (1<<8) ; 
		pI2Cx->pI2Cx->CR2 |= (1<<10) ;
		pI2Cx->pI2Cx->CR2 |= (1<<9) ;
		pI2Cx->pI2Cx->CR2 |= (1<<8) ;

	}

	return busystate ;

}
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2Cx , uint8_t  *pRxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr , uint8_t Sr) {

	uint8_t busystate = pI2Cx->TxRxState ;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX) ){

		pI2Cx->pRxBuffer = pRxBuffer ;
		pI2Cx->RxLen = len ;
		pI2Cx->TxRxState = I2C_BUSY_IN_RX ;
		pI2Cx->RxSize = len ;   
		pI2Cx->DevAddr = SlaveAddr ;

	
	 pI2Cx->pI2Cx->CR1 |= (1<<8) ;
     pI2Cx->pI2Cx->CR2 |= (1<<10) ;
     pI2Cx->pI2Cx->CR2 |= (1<<9) ; 
	 pI2Cx->pI2Cx->CR2 |= (1<<8) ;

	}

	return busystate ;

}

static void I2C_CloseSendData(I2C_Handle_t *pI2CHandle ){

	//  disable ITBUFEN and ITEVTEN , transfer is done ;
	pI2CHandle->pI2Cx->CR2 &= ~(1<<10) ;
	pI2CHandle->pI2Cx->CR2 &= ~(1<<9) ;

	pI2CHandle->TxRxState = I2C_READY ;
	pI2CHandle->pTxBuffer = NULL ;
	pI2CHandle->TxLen = 0 ;
}

static void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle){

	pI2CHandle->pI2Cx->CR2 &= ~(1<<10) ;
	pI2CHandle->pI2Cx->CR2 &= ~(1<<9) ;

	pI2CHandle->TxRxState = I2C_READY ;
	pI2CHandle->pRxBuffer = NULL ;
	pI2CHandle->RxLen = 0 ;
	pI2CHandle->RxSize = 0 ;

	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE){
		I2C_ManageACK(pI2CHandle , ENABLE) ;
	}
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle , uint8_t AppEv  ) ;


void I2C_IRQConfig(uint8_t IRQ_Number ,   uint8_t EnOrDi){

	if(EnOrDi == ENABLE){
		if(IRQ_Number <=31){

			*NVIC_ISER0 |= (1<<IRQ_Number) ;
		}
		else if(IRQ_Number >31 && IRQ_Number <64 ){
	
			*NVIC_ISER1 |= (1<< (IRQ_Number %32) ) ;
		}
		else if(IRQ_Number >=64 && IRQ_Number <96 ){

			*NVIC_ISER2 |= (1<< (IRQ_Number %64) ) ;
		}
	}
	else{
		if(IRQ_Number <=31){

			*NVIC_ICER0 |= (1<<IRQ_Number) ;
		}
		else if(IRQ_Number >31 && IRQ_Number <64 ){

			*NVIC_ICER1 |= (1<< (IRQ_Number %32) ) ;
		}
		else if(IRQ_Number >=64 && IRQ_Number <96 ){
		
			*NVIC_ICER2 |= (1<< (IRQ_Number %64) ) ;
		}
	}
}
void I2C_IRQPriorityConfig(uint8_t  IRQNumber , uint8_t IRQPriority) {

	//  find the IPR register ;
	uint8_t iprx =  IRQNumber /4 ;
	uint8_t iprx_section =  IRQNumber %4 ;

	uint8_t shift_amount = (8*iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED) ;

	*(NVIC_PR_BASE_ADDR + iprx) |=  (IRQPriority << shift_amount) ;
}

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle){

	uint32_t temp1 , temp2 , temp3 ;

	temp1 =  pI2CHandle->pI2Cx->CR2 & (1<<9)  ;   
	temp2 =  pI2CHandle->pI2Cx->CR2 & (1<<10) ;
	temp3 =  pI2CHandle->pI2Cx->SR1 & (I2C_FLAG_SB) ;

	if(temp1 && temp3){
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
			pI2CHandle->pI2Cx->DR = (pI2CHandle->DevAddr <<1) & ~(1) ;
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			pI2CHandle->pI2Cx->DR = (pI2CHandle->DevAddr <<1) | 1 ;
		}
	}

	temp3 =  pI2CHandle->pI2Cx->SR1 & (I2C_FLAG_ADDR) ;

	if(temp1 && temp3){

		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX && pI2CHandle->RxSize ==1){
			I2C_ManageACK(pI2CHandle , DISABLE) ;
		}
		
		uint32_t dummyread = pI2CHandle->pI2Cx->SR1 ;
		dummyread =  pI2CHandle->pI2Cx->SR2 ;
		(void)dummyread ;
	}

	temp3 =  pI2CHandle->pI2Cx->SR1 & (I2C_FLAG_BTF) ;

	if(temp1 && temp3){
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
			if(pI2CHandle->pI2Cx->SR1 & I2C_FLAG_TXE){
				//  BTF and TXE both  set  ,  txn done ;
				if(pI2CHandle->TxLen ==0){
					pI2CHandle->pI2Cx->CR1 |= (1<<9) ;   //  generate stop ;
					I2C_CloseSendData(pI2CHandle) ;
					I2C_ApplicationEventCallback(pI2CHandle , I2C_EV_TX_CMPLT) ;
				}
			}
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){
			; //  do nothing 
		}
	}

	temp3 =  pI2CHandle->pI2Cx->SR1 & (I2C_FLAG_STOPF) ;
	if(temp1 && temp3){
		pI2CHandle->pI2Cx->CR1 |= 0x0000 ;
		I2C_ApplicationEventCallback(pI2CHandle , I2C_EV_STOP) ;
	}

	temp3 =  pI2CHandle->pI2Cx->SR1 & (I2C_FLAG_TXE) ;

	if(temp1 && temp2 && temp3){
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX){
			if(pI2CHandle->TxLen >0){
				pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer) ;
				pI2CHandle->TxLen-- ;
				pI2CHandle->pTxBuffer++ ;
			}
		}
	}

	temp3 =  pI2CHandle->pI2Cx->SR1 & (I2C_FLAG_RXNE) ;

	if(temp1 && temp2 && temp3){
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX){

			if(pI2CHandle->RxSize ==1){
				*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->DR ;
				pI2CHandle->RxLen-- ;
			}
			else if(pI2CHandle->RxSize >1){
				if(pI2CHandle->RxLen ==2){
					I2C_ManageACK(pI2CHandle , DISABLE) ;
				}
				*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->DR ;
				pI2CHandle->pRxBuffer++ ;
				pI2CHandle->RxLen-- ;
			}

			if(pI2CHandle->RxLen ==0){
				pI2CHandle->pI2Cx->CR1 |= (1<<9) ;  
				I2C_CloseReceiveData(pI2CHandle) ;
				I2C_ApplicationEventCallback(pI2CHandle , I2C_EV_RX_CMPLT) ;
			}
		}
	}
}

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle){

	uint32_t temp1 , temp2 ;

	temp2 = pI2CHandle->pI2Cx->CR2 & (1<<8) ;   //  ITERREN status ;


	temp1 = pI2CHandle->pI2Cx->SR1 & I2C_FLAG_BERR ;
	if(temp1 && temp2){
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_FLAG_BERR) ;
		I2C_ApplicationEventCallback(pI2CHandle , I2C_ERROR_BERR) ;
	}


	temp1 = pI2CHandle->pI2Cx->SR1 & I2C_FLAG_ARLO ;
	if(temp1 && temp2){
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_FLAG_ARLO) ;
		I2C_ApplicationEventCallback(pI2CHandle , I2C_ERROR_ARLO) ;
	}


	temp1 = pI2CHandle->pI2Cx->SR1 & I2C_FLAG_AF ;
	if(temp1 && temp2){
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_FLAG_AF) ;
		I2C_ApplicationEventCallback(pI2CHandle , I2C_ERROR_AF) ;
	}


	temp1 = pI2CHandle->pI2Cx->SR1 & I2C_FLAG_OVR ;
	if(temp1 && temp2){
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_FLAG_OVR) ;
		I2C_ApplicationEventCallback(pI2CHandle , I2C_ERROR_OVR) ;
	}

	temp1 = pI2CHandle->pI2Cx->SR1 & I2C_FLAG_TIMEOUT ;
	if(temp1 && temp2){
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_FLAG_TIMEOUT) ;
		I2C_ApplicationEventCallback(pI2CHandle , I2C_ERROR_TIMEOUT) ;
	}
}

void I2C_IRQConfig(uint8_t IRQ_Number ,   uint8_t EnOrDi);
void I2C_IRQPriorityConfig(uint8_t  IRQNumber , uint8_t IRQPriority) ;
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle) ;
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle) ;

#endif 
