DEVICE := NRF51
DEVICESERIES := nrf51

SDK_INCLUDE_PATH = $(SDK_PATH)/Include/
SDK_SOURCE_PATH = $(SDK_PATH)/Source/
TEMPLATE_PATH += $(SDK_SOURCE_PATH)/templates/gcc/
OUTPUT_BINARY_DIRECTORY := _build

ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)Makefile.windows
else
include $(TEMPLATE_PATH)Makefile.posix
endif

ifeq ($(LINKER_SCRIPT),)
	ifeq ($(USE_SOFTDEVICE), S110)
		LINKER_SCRIPT = gcc_$(DEVICESERIES)_s110_$(DEVICE_VARIANT).ld
		OUTPUT_FILENAME := $(OUTPUT_FILENAME)_s110_$(DEVICE_VARIANT)
	else
		ifeq ($(USE_SOFTDEVICE), S210)
			LINKER_SCRIPT = gcc_$(DEVICESERIES)_s210_$(DEVICE_VARIANT).ld
			OUTPUT_FILENAME := $(OUTPUT_FILENAME)_s210_$(DEVICE_VARIANT)
		else
			LINKER_SCRIPT = gcc_$(DEVICESERIES)_blank_$(DEVICE_VARIANT).ld
			OUTPUT_FILENAME := $(OUTPUT_FILENAME)_$(DEVICE_VARIANT)
		endif
	endif
else
# Use externally defined settings
endif

CPU := cortex-m0

# Toolchain commands
CC       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc"
AS       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as"
AR       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld"
NM       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy"

MK 				:= mkdir
RM 				:= rm -rf

OBJECT_DIRECTORY := _build
LISTING_DIRECTORY := _build

C_SOURCE_FILES += system_$(DEVICESERIES).c
ASSEMBLER_SOURCE_FILES += gcc_startup_$(DEVICESERIES).s

# Linker flags
#LDFLAGS += -L"$(GNU_INSTALL_ROOT)/arm-none-eabi/lib/armv6-m"
#LDFLAGS += -L"$(GNU_INSTALL_ROOT)/lib/gcc/arm-none-eabi/$(GNU_VERSION)/armv6-m"
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mcpu=$(CPU) -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT) 

# Compiler flags
CFLAGS += -mcpu=$(CPU) -mthumb -mabi=aapcs -D$(DEVICE) -D$(BOARD) -D$(TARGET_CHIP) --std=gnu99
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=soft

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
 
INCLUDEPATHS += -I"../"
INCLUDEPATHS += -I"$(SDK_PATH)Include"
INCLUDEPATHS += -I"$(SDK_PATH)Include/gcc"
INCLUDEPATHS += -I"$(SDK_PATH)Include/ext_sensors"

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

####################################################################
# Rules                                                            #
####################################################################

C_SOURCE_FILENAMES = $(notdir $(C_SOURCE_FILES) )
ASSEMBLER_SOURCE_FILENAMES = $(notdir $(ASSEMBLER_SOURCE_FILES) )

# Make a list of source paths
C_SOURCE_PATHS += ../ $(SDK_SOURCE_PATH) $(TEMPLATE_PATH) $(wildcard $(SDK_SOURCE_PATH)*/)  $(wildcard $(SDK_SOURCE_PATH)ext_sensors/*/) $(wildcard $(SDK_SOURCE_PATH)ble/*/)
ASSEMBLER_SOURCE_PATHS = ../ $(SDK_SOURCE_PATH) $(TEMPLATE_PATH) $(wildcard $(SDK_SOURCE_PATH)*/)

C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILENAMES:.c=.o) )
ASSEMBLER_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASSEMBLER_SOURCE_FILENAMES:.s=.o) )

# Set source lookup paths
vpath %.c $(C_SOURCE_PATHS)
vpath %.s $(ASSEMBLER_SOURCE_PATHS)

# Include automatically previously generated dependencies
-include $(addprefix $(OBJECT_DIRECTORY)/, $(COBJS:.o=.d))

### Targets
OBdebug:    CFLAGS += -DDEBUG -g3 -O0
debug:    ASMFLAGS += -DDEBUG -g3 -O0
debug:    $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

.PHONY: release
release: clean
release:  CFLAGS += -DNDEBUG -O3
release:  ASMFLAGS += -DNDEBUG -O3
release:  $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echostuff:
	@echo C_OBJECTS: [$(C_OBJECTS)]
	@echo C_SOURCE_FILES: [$(C_SOURCE_FILES)]
	@echo C_SOURCE_PATHS: [$(C_SOURCE_PATHS)]

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

## Create objects from C source files
$(OBJECT_DIRECTORY)/%.o: %.c
# Build header dependencies
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -M $< -MF "$(@:.o=.d)" -MT $@
# Do the actual compilation
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Assemble .s files
$(OBJECT_DIRECTORY)/%.o: %.s
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Link C and assembler objects to an .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(C_OBJECTS) $(ASSEMBLER_OBJECTS) $(LIBRARIES)
	$(CC) $(LDFLAGS) $(C_OBJECTS) $(ASSEMBLER_OBJECTS) $(LIBRARIES) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

## Default build target
.PHONY: all
all: clean release

clean:
	$(RM) $(OUTPUT_BINARY_DIRECTORY)

## Program device
.PHONY: flash
flash: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
	nrfjprog --reset --program $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

.PHONY: reset
reset:
	nrfjprog --reset

.PHONY: erase
erase:
	nrfjprog --erase
