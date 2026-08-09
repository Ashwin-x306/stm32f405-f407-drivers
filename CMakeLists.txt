cmake_minimum_required(VERSION 4.3)
project(drivers_file C)

set(CMAKE_C_STANDARD 11)

include_directories(drivers/Inc)

add_executable(drivers_file
        drivers/Inc/image.h
        drivers/Inc/LIS3DSH.h
        drivers/Inc/ST7789.h
        drivers/Inc/stm32f405xxx.h
        drivers/Inc/stm32f405xxx_adc_driver.h
        drivers/Inc/stm32f405xxx_gpio_driver.h
        drivers/Inc/stm32f405xxx_i2c_driver.h
        drivers/Inc/stm32f405xxx_spi_driver.h
        drivers/Inc/sys_clock.h
        drivers/Src/LIS3DSH.c
        drivers/Src/ST7789.c
        drivers/Src/stm32f405xxx_adc_driver.c
        drivers/Src/stm32f405xxx_gpio_driver.c
        drivers/Src/stm32f405xxx_i2c_driver.c
        drivers/Src/stm32f405xxx_spi_driver.c
        drivers/Src/sys_clock.c
        Src/int_bas_ledtog.c
        Src/main.c
        Src/main_two.c
        Src/spi_communication_tut.c
        Src/syscalls.c
        Src/sysmem.c)
