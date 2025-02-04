################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Alt_fuggvenyek.c \
../Core/Src/BK2_setting_menu.c \
../Core/Src/CGOL.c \
../Core/Src/Fonts_and_bitmaps_FLASH.c \
../Core/Src/M95010_W_EEPROM.c \
../Core/Src/Minesweepergame.c \
../Core/Src/ST7565_64x128_LCD.c \
../Core/Src/Tetrisgame.c \
../Core/Src/disp_fgv.c \
../Core/Src/main.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/trexgame.c \
../Core/Src/trexgame_bitmaps_FLASH.c 

OBJS += \
./Core/Src/Alt_fuggvenyek.o \
./Core/Src/BK2_setting_menu.o \
./Core/Src/CGOL.o \
./Core/Src/Fonts_and_bitmaps_FLASH.o \
./Core/Src/M95010_W_EEPROM.o \
./Core/Src/Minesweepergame.o \
./Core/Src/ST7565_64x128_LCD.o \
./Core/Src/Tetrisgame.o \
./Core/Src/disp_fgv.o \
./Core/Src/main.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/trexgame.o \
./Core/Src/trexgame_bitmaps_FLASH.o 

C_DEPS += \
./Core/Src/Alt_fuggvenyek.d \
./Core/Src/BK2_setting_menu.d \
./Core/Src/CGOL.d \
./Core/Src/Fonts_and_bitmaps_FLASH.d \
./Core/Src/M95010_W_EEPROM.d \
./Core/Src/Minesweepergame.d \
./Core/Src/ST7565_64x128_LCD.d \
./Core/Src/Tetrisgame.d \
./Core/Src/disp_fgv.d \
./Core/Src/main.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/trexgame.d \
./Core/Src/trexgame_bitmaps_FLASH.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L412xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Alt_fuggvenyek.cyclo ./Core/Src/Alt_fuggvenyek.d ./Core/Src/Alt_fuggvenyek.o ./Core/Src/Alt_fuggvenyek.su ./Core/Src/BK2_setting_menu.cyclo ./Core/Src/BK2_setting_menu.d ./Core/Src/BK2_setting_menu.o ./Core/Src/BK2_setting_menu.su ./Core/Src/CGOL.cyclo ./Core/Src/CGOL.d ./Core/Src/CGOL.o ./Core/Src/CGOL.su ./Core/Src/Fonts_and_bitmaps_FLASH.cyclo ./Core/Src/Fonts_and_bitmaps_FLASH.d ./Core/Src/Fonts_and_bitmaps_FLASH.o ./Core/Src/Fonts_and_bitmaps_FLASH.su ./Core/Src/M95010_W_EEPROM.cyclo ./Core/Src/M95010_W_EEPROM.d ./Core/Src/M95010_W_EEPROM.o ./Core/Src/M95010_W_EEPROM.su ./Core/Src/Minesweepergame.cyclo ./Core/Src/Minesweepergame.d ./Core/Src/Minesweepergame.o ./Core/Src/Minesweepergame.su ./Core/Src/ST7565_64x128_LCD.cyclo ./Core/Src/ST7565_64x128_LCD.d ./Core/Src/ST7565_64x128_LCD.o ./Core/Src/ST7565_64x128_LCD.su ./Core/Src/Tetrisgame.cyclo ./Core/Src/Tetrisgame.d ./Core/Src/Tetrisgame.o ./Core/Src/Tetrisgame.su ./Core/Src/disp_fgv.cyclo ./Core/Src/disp_fgv.d ./Core/Src/disp_fgv.o ./Core/Src/disp_fgv.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/trexgame.cyclo ./Core/Src/trexgame.d ./Core/Src/trexgame.o ./Core/Src/trexgame.su ./Core/Src/trexgame_bitmaps_FLASH.cyclo ./Core/Src/trexgame_bitmaps_FLASH.d ./Core/Src/trexgame_bitmaps_FLASH.o ./Core/Src/trexgame_bitmaps_FLASH.su

.PHONY: clean-Core-2f-Src

