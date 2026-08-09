/*
 * stm32f405xxx_i2c_driver.h
 *
 *  Created on: Jul 23, 2026
 *      Author: Ashwin
 */

#ifndef INC_STM32F405XXX_I2C_DRIVER_H_
#define INC_STM32F405XXX_I2C_DRIVER_H_
#include"stm32f405xxx.h"



typedef struct{

	uint32_t I2C_SCLSpeed ;
	uint8_t I2C_DeviceAddress ; // if in slave mode
	uint8_t  I2C_ACKControl ;
	uint16_t I2C_FMDutyCycle ;
}I2C_Config_t;


typedef struct{

	I2C_RegDef_t *pI2Cx ;
	I2C_Config_t I2C_Config ;
	uint8_t *pTxBuffer ; 
	uint8_t  *pRxBuffer ; 
	uint32_t TxLen ; 
	uint32_t RxLen ; 
	uint8_t  TxRxState ;
	uint8_t DevAddr ; 
	uint32_t  RxSize ; 
	uint8_t Sr ; 
}I2C_Handle_t;


#define I2C_SCL_SPEED_SM   100000
#define I2C_SCL_SPEED_FM4K 400000
#define I2C_SCL_SPEED_FM2K 200000



#define I2C_ACK_ENABLE   1
#define I2C_ACK_DISBALE  0


#define I2C_FM_DUTY_2    0
#define IC2_FM_DUTY_16_9 1
#define I2C_SR1_SB          0
#define I2C_SR1_ADDR        1
#define I2C_SR1_BTF         2
#define I2C_SR1_ADD10       3
#define I2C_SR1_STOPF       4
#define I2C_SR1_RXNE        6
#define I2C_SR1_TXE         7
#define I2C_SR1_BERR        8
#define I2C_SR1_ARLO        9
#define I2C_SR1_AF          10
#define I2C_SR1_OVR         11
#define I2C_SR1_PECERR      12
#define I2C_SR1_TIMEOUT     14
#define I2C_SR1_SMBALERT    15

/*
 * I2C related status flags definitions
 */
#define I2C_FLAG_SB         (1U << I2C_SR1_SB)
#define I2C_FLAG_ADDR       (1U << I2C_SR1_ADDR)
#define I2C_FLAG_BTF        (1U << I2C_SR1_BTF)
#define I2C_FLAG_ADD10      (1U << I2C_SR1_ADD10)
#define I2C_FLAG_STOPF      (1U << I2C_SR1_STOPF)
#define I2C_FLAG_RXNE       (1U << I2C_SR1_RXNE)
#define I2C_FLAG_TXE        (1U << I2C_SR1_TXE)
#define I2C_FLAG_BERR       (1U << I2C_SR1_BERR)
#define I2C_FLAG_ARLO       (1U << I2C_SR1_ARLO)
#define I2C_FLAG_AF         (1U << I2C_SR1_AF)
#define I2C_FLAG_OVR        (1U << I2C_SR1_OVR)
#define I2C_FLAG_PECERR     (1U << I2C_SR1_PECERR)
#define I2C_FLAG_TIMEOUT    (1U << I2C_SR1_TIMEOUT)
#define I2C_FLAG_SMBALERT   (1U << I2C_SR1_SMBALERT) 

#define NO_PR_BITS_IMPLEMENTED   4

#define I2C_READY             1 

#define I2C_BUSY_IN_RX        2 

#define I2C_BUSY_IN_TX        3
#define I2C_CR2_ITBUFEN       10
#define I2C_EV_TX_CMPLT                1
#define I2C_EV_RX_CMPLT                2
#define I2C_EV_STOP                    3
#define I2C_ERROR_BERR                 4
#define I2C_ERROR_ARLO                 5
#define I2C_ERROR_AF                   6
#define I2C_ERROR_OVR                  7
#define I2C_ERROR_TIMEOUT              8

void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx   , uint8_t EnORDi) ;
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx  , uint8_t EnorDi) ;
void I2C_MasterSendData(I2C_Handle_t *pI2Cx , uint8_t  *pTxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr) ;
void I2C_MasterReceiveData(I2C_Handle_t *pI2Cx , uint8_t  *pRxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr) ;  

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2Cx , uint8_t  *pTxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr, uint8_t Sr) ;
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2Cx , uint8_t  *pRxBuffer ,  uint32_t len  ,  uint8_t SlaveAddr , uint8_t Sr) ;  


void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle , uint8_t AppEv  ) ;


void I2C_IRQConfig(uint8_t IRQ_Number ,   uint8_t EnOrDi);
void I2C_IRQPriorityConfig(uint8_t  IRQNumber , uint8_t IRQPriority) ;


#endif /* INC_STM32F405XXX_I2C_DRIVER_H_ */
