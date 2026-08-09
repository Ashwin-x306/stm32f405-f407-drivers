/*
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


	temp = ((RCC->CFGR >>4)&0xF) ;
	if(temp<8)ahbp = 1;

	else ahbp = AHB_PreScalar[temp - 8] ;

	temp = ((RCC->CFGR>>4)&0x7) ;
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
       	pI2CHandle->pI2Cx->CCR = tempreg;

    }
    // this  is still pending ;
}
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle , uint8_t *pTxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr) {

	//  generate the start condition;
	pI2CHandle->pI2Cx->CR1 = (1<<8) ;
    while(!(GetFlagStatus (pI2CHandle  ,  1  , I2C_FLAG_SB))) ;  // wait till it sets;

    SlaveAddr =  SlaveAddr<<1 ;
    SlaveAddr &=~(1) ;
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

}





























#endif /* SRC_STM32F405XXX_I2C_DRIVER_C_ */
