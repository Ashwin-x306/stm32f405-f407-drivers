# STM32F405-F407-Drivers

This repository contains the bare-metal drivers I'm building from scratch for the STM32F405/F407 microcontrollers.

I'm writing these drivers mainly to understand what is actually happening inside the MCU at the register level, instead of depending on STM32 HAL or LL libraries.

The idea is to build the peripheral drivers myself, starting from the memory map and register definitions and then building the driver APIs on top of them.

---

## Why this project?

When working with STM32, it is very easy to configure peripherals using HAL and get things working quickly. However, I wanted to understand what happens underneath those APIs.

So in this project I'm working directly with:

- Peripheral memory addresses
- Register definitions
- Register bit fields
- RCC clock configuration
- GPIO alternate functions
- Peripheral initialization
- Interrupt configuration
- Data transmission and reception

The goal is not just to make the peripherals work, but to understand how they work.

---

## Current Drivers

The drivers I have worked on / am working on include:

### GPIO

The GPIO driver handles:

- GPIO initialization
- Input and output configuration
- Pull-up / pull-down configuration
- Output speed
- Output type
- Alternate function configuration
- Reading input pins
- Writing output pins
- Toggling output pins
- GPIO interrupts

### SPI

The SPI driver handles:

- SPI initialization
- SPI clock configuration
- SPI mode configuration
- Clock polarity and phase
- Data frame configuration
- Full-duplex communication
- SPI transmit and receive operations
- SPI status flags
- Interrupt configuration

### I2C

The I2C driver is being developed to handle:

- I2C initialization
- Clock configuration
- Device addressing
- Start and stop conditions
- Master transmit
- Master receive
- Status flags
- Interrupt handling

### ADC

The ADC driver handles the basic ADC functionality required for reading analog signals.

This includes:

- ADC initialization
- Resolution configuration
- Channel configuration
- Sampling configuration
- Starting conversions
- Reading ADC values
- Converting ADC values to voltage

### USART

The USART driver is currently being developed.

The plan is to support:

- Baud rate configuration
- Word length
- Stop bits
- Parity
- Transmitter
- Receiver
- Hardware flow control
- Status flags
- Interrupts

### EXTI / Interrupts

Interrupt configuration is also handled at the register level.

This includes:

- EXTI line configuration
- Rising-edge interrupts
- Falling-edge interrupts
- NVIC configuration
- Interrupt priority
- Interrupt handling

---

## Project Structure

The project is organized into MCU-specific definitions and peripheral drivers.

```text
STM32F405-F407-Drivers/
│
├── drivers/
│   │
│   ├── Inc/
│   │   ├── stm32f405xxx.h
│   │   ├── stm32f405xxx_gpio_driver.h
│   │   ├── stm32f405xxx_spi_driver.h
│   │   ├── stm32f405xxx_i2c_driver.h
│   │   ├── stm32f405xxx_adc_driver.h
│   │   └── stm32f405xxx_usart_driver.h
│   │
│   └── Src/
│       ├── stm32f405xxx_gpio_driver.c
│       ├── stm32f405xxx_spi_driver.c
│       ├── stm32f405xxx_i2c_driver.c
│       ├── stm32f405xxx_adc_driver.c
│       └── stm32f405xxx_usart_driver.c
│
├── Startup/
│
├
│
└── README.md
