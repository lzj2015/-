################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/eccdrbg.cpp \
../src/lattdrbg.cpp \
../src/main.cpp 

C_SRCS += \
../src/sha256.c \
../src/timing.c 

OBJS += \
./src/eccdrbg.o \
./src/lattdrbg.o \
./src/main.o \
./src/sha256.o \
./src/timing.o 

CPP_DEPS += \
./src/eccdrbg.d \
./src/lattdrbg.d \
./src/main.d 

C_DEPS += \
./src/sha256.d \
./src/timing.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O2 -g3 -Wall -c -fmessage-length=0 -lgmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O2 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


