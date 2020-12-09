################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include testing/subdir.mk
-include Visitors/subdir.mk
-include Sym/subdir.mk
-include Parse/subdir.mk
-include AST/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CPPM_DEPS)),)
-include $(CPPM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(BC_DEPS)),)
-include $(BC_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LLVM_EXECUTABLES += \
USER_OBJS.exe \


# All Target
all: Cappa secondary-outputs

# Tool invocations
Cappa: $(BCS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: LLVM Clang C++ linker'
	clang++ -disable-internalize -disable-opt -disable-inlining -rdynamic -fno-exceptions -fno-rtti -L/home/chris/eclipse-workspace/llvm/build/lib  -lLLVM-11 -O0 -o "Cappa" $(BCS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

USER_OBJS.exe: Cappa $(BCS) $(USER_OBJS)

# Other Targets
clean:
	-$(RM) $(CPPM_DEPS)$(BCS)$(CC_DEPS)$(C++_DEPS)$(LLVM_BC_EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(BC_DEPS)$(LLVM_EXECUTABLES)$(C_DEPS)$(CPP_DEPS) Cappa
	-@echo ' '

secondary-outputs: $(LLVM_BC_EXECUTABLES)

.PHONY: all clean dependents

-include ../makefile.targets
