################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Visitors/CodegenVisitor.cpp \
../Visitors/SigGenVisitor.cpp \
../Visitors/TypeDefVisitor.cpp 

BCS += \
./Visitors/CodegenVisitor.bc \
./Visitors/SigGenVisitor.bc \
./Visitors/TypeDefVisitor.bc 

CPP_DEPS += \
./Visitors/CodegenVisitor.d \
./Visitors/SigGenVisitor.d \
./Visitors/TypeDefVisitor.d 


# Each subdirectory must supply rules for building sources it contributes
Visitors/%.bc: ../Visitors/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: LLVM Clang++'
	clang++ -I/usr/include/llvm-11 -I/usr/include/llvm-c-11 -I"/home/chris/eclipse-workspace/Cappa" -O0 -emit-llvm -g3 -Wall -c -fmessage-length=0 -fno-exceptions -frtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -std=c++17 -MMD -MP -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


