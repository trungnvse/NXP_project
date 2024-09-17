################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/app/src/app.c 

C_DEPS += \
./source/app/src/app.d 

OBJS += \
./source/app/src/app.o 


# Each subdirectory must supply rules for building sources it contributes
source/app/src/%.o: ../source/app/src/%.c source/app/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKE16Z64VLF4 -DCPU_MKE16Z64VLF4_cm0plus -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source\app\inc" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source\uart\inc" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source\srec\inc" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source\queue\inc" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source\flash\inc" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\source\boot\inc" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\device" -I"E:\FPT ACADEMY\MCU\CODE\Nguyen_Viet_Trung_MockMCU\CMSIS" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-app-2f-src

clean-source-2f-app-2f-src:
	-$(RM) ./source/app/src/app.d ./source/app/src/app.o

.PHONY: clean-source-2f-app-2f-src

