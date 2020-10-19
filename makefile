# STM32G0 Bootloader
# Jonah Swain
# Makefile

# ======== CONFIGURATION SECTION ========

# === TARGETS ===
TARGET_DIR = outputs
BL_TARGET = bootloader
APP_1_TARGET = application-1
APP_2_TARGET = application-2

# === OPTIONS ===
# Enable map file outputs (true/fase)
OUTPUT_MAPS = true

# === BOOTLOADER CONFIG ===
# Bootloader directories
BL_BASEDIR = bootloader
BL_SRCDIR = $(BL_BASEDIR)/src
BL_INCDIR = $(BL_BASEDIR)/include
BL_OBJDIR = $(BL_BASEDIR)/obj
BL_ASMDIR = $(BL_BASEDIR)/asm

# Bootloader linker script
BL_LDSCRIPT = $(BL_BASEDIR)/bootloader.ld

# === APPLICATION CONFIG ===
# Application directories
APP_BASEDIR = application
APP_SRCDIR = $(APP_BASEDIR)/src
APP_INCDIR = $(APP_BASEDIR)/include
APP_OBJDIR = $(APP_BASEDIR)/obj
APP_ASMDIR = $(APP_BASEDIR)/asm

# Application linker scripts
APP1_LDSCRIPT = $(APP_BASEDIR)/appspace_1.ld
APP2_LDSCRIPT = $(APP_BASEDIR)/appspace_2.ld

# === LIBRARY CONFIG ===
# Library directories
LIB_SRCDIRS += drivers/STM32G0xx_HAL_Driver/Src
LIB_INCDIRS += common
LIB_INCDIRS += drivers/CMSIS/Include
LIB_INCDIRS += drivers/CMSIS/Device/ST/STM32G0xx/Include
LIB_INCDIRS += drivers/STM32G0xx_HAL_Driver/Inc
LIB_OBJDIR = drivers/obj

# === COMPILER, ASSEMBLER & LINKER CONFIG ===
# C Cross compiler package
CROSS_COMPILER = arm-none-eabi-

# C standard
CSTD = c18

# Optimisation level
OPTLVL = O0

# Device and processor
DEVICE = STM32G071xx
CPU = cortex-m0plus


# ====== END CONFIGURATION SECTION ======
# Do not change anything below this line unless you know what you're doing

# File extensions/suffixes
.SUFFIXES: .c .h .s .o .elf .hex .bin

# Phony rules (no dependencies)
.PHONY: all clean clean_all bootloader clean_bootloader applications application_1 application_2 clean_applications clean_libs

# Define newline
define \n


endef

# Compiler, linker, debugger, assembler, and object utilities
CC = $(CROSS_COMPILER)gcc
LD = $(CROSS_COMPILER)ld
GDB = $(CROSS_COMPILER)gdb
AS = $(CROSS_COMPILER)as
OCPY = $(CROSS_COMPILER)objcopy
ODMP = $(CROSS_COMPILER)objdump
SIZE = $(CROSS_COMPILER)size

# Compiler flags
CCFLAGS += -mcpu=$(CPU) -mthumb
CCFLAGS += -std=$(CSTD) -$(OPTLVL)
CCFLAGS += -Wall -Werror -Wno-unused-function -Wno-address-of-packed-member
CCFLAGS += -ffreestanding -ffunction-sections -fdata-sections
CCFLAGS += -g
CCFLAGS += $(foreach lib,$(LIB_INCDIRS), -I$(lib))
CCFLAGS += -D$(DEVICE) -DUSE_HAL_DRIVER

# Linker flags
LDFLAGS += -mcpu=$(CPU) -mthumb
LDFLAGS += -Wall -Werror
LDFLAGS += -nostdlib -lgcc -nostartfiles
LDFLAGS += --specs=nano.specs --specs=nosys.specs
LDFLAGS += -Wl,--gc-sections

# Assembler flags
ASFLAGS += -mcpu=$(CPU) -mthumb -c


# Bootloader source and object files
BL_C_SRCS := $(foreach dir,$(BL_SRCDIR),$(wildcard $(dir)/*.c))
BL_ASM_SRCS := $(foreach dir,$(BL_ASMDIR),$(wildcard $(dir)/*.s))
BL_OBJS := $(foreach src,$(BL_C_SRCS),$(BL_OBJDIR)/$(notdir $(src:%.c=%.o)))
BL_OBJS += $(foreach src,$(BL_ASM_SRCS),$(BL_OBJDIR)/$(notdir $(src:%.s=%.o)))

# Application source and object files
APP_C_SRCS := $(foreach dir,$(APP_SRCDIR),$(wildcard $(dir)/*.c))
APP_ASM_SRCS := $(foreach dir,$(APP_ASMDIR),$(wildcard $(dir)/*.s))
APP_OBJS := $(foreach src,$(APP_C_SRCS),$(APP_OBJDIR)/$(notdir $(src:%.c=%.o)))
APP_OBJS += $(foreach src,$(APP_ASM_SRCS),$(APP_OBJDIR)/$(notdir $(src:%.s=%.o)))

# Library source and object files
LIB_SRCS := $(foreach dir,$(LIB_SRCDIRS),$(wildcard $(dir)/*.c))
LIB_OBJS := $(foreach src,$(LIB_SRCS),$(LIB_OBJDIR)/$(notdir $(src:%.c=%.o)))


# ======== BUILD RULES ========

# Build all (bootloader and application for both application spaces)
all: bootloader application_1 application_2

# Clean all build files
clean: clean_bootloader clean_applications clean_libs
clean_all: clean_bootloader clean_applications clean_libs

# === BOOTLOADER BUILD RULES ===
# Build bootloader (all)
bootloader: $(TARGET_DIR)/$(BL_TARGET).hex $(TARGET_DIR)/$(BL_TARGET).bin | $(TARGET_DIR)

# Clean bootloader files
clean_bootloader:
	rm -f $(TARGET_DIR)/$(BL_TARGET).elf
	rm -f $(TARGET_DIR)/$(BL_TARGET).hex
	rm -f $(TARGET_DIR)/$(BL_TARGET).bin
	rm -f $(TARGET_DIR)/$(BL_TARGET).map
	rm -f $(BL_OBJS)

# Bootloader C sources
$(BL_OBJDIR)/%.o: $(BL_SRCDIR)/%.c | $(BL_OBJDIR)
	$(CC) $(CCFLAGS) -I$(BL_INCDIR) -c $< -o $@

# Bootloader asm sources
$(BL_OBJDIR)/%.o: $(BL_ASMDIR)/%.s | $(BL_OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

# Bootloader ELF
$(TARGET_DIR)/$(BL_TARGET).elf: $(BL_OBJS) $(LIB_OBJS) | $(TARGET_DIR)
ifeq ($(OUTPUT_MAPS), true)
	$(CC) $(LDFLAGS) -Xlinker -Map=$(TARGET_DIR)/$(BL_TARGET).map -T $(BL_LDSCRIPT) $^ -o $@
else
	$(CC) $(LDFLAGS) -T $(BL_LDSCRIPT) $^ -o $@
endif

# Bootloader binary
$(TARGET_DIR)/$(BL_TARGET).bin: $(TARGET_DIR)/$(BL_TARGET).elf | $(TARGET_DIR)
	$(OCPY) -O binary $< $@

# Bootloader HEX
$(TARGET_DIR)/$(BL_TARGET).hex: $(TARGET_DIR)/$(BL_TARGET).elf | $(TARGET_DIR)
	$(OCPY) -O ihex $< $@


# === APPLICATION BUILD RULES ===
# Build both applications
applications: $(TARGET_DIR)/$(APP_1_TARGET).hex $(TARGET_DIR)/$(APP_1_TARGET).bin $(TARGET_DIR)/$(APP_2_TARGET).hex $(TARGET_DIR)/$(APP_2_TARGET).bin | $(TARGET_DIR)

# Build application 1
application_1: $(TARGET_DIR)/$(APP_1_TARGET).hex $(TARGET_DIR)/$(APP_1_TARGET).bin | $(TARGET_DIR)

# Build application 2
application_2: $(TARGET_DIR)/$(APP_2_TARGET).hex $(TARGET_DIR)/$(APP_2_TARGET).bin | $(TARGET_DIR)

# Clean application build files
clean_applications:
	rm -f $(TARGET_DIR)/$(APP_1_TARGET).elf
	rm -f $(TARGET_DIR)/$(APP_1_TARGET).hex
	rm -f $(TARGET_DIR)/$(APP_1_TARGET).bin
	rm -f $(TARGET_DIR)/$(APP_1_TARGET).map
	rm -f $(TARGET_DIR)/$(APP_2_TARGET).elf
	rm -f $(TARGET_DIR)/$(APP_2_TARGET).hex
	rm -f $(TARGET_DIR)/$(APP_2_TARGET).bin
	rm -f $(TARGET_DIR)/$(APP_2_TARGET).map
	rm -f $(APP_OBJS)

# Application C sources
$(APP_OBJDIR)/%.o: $(APP_SRCDIR)/%.c | $(APP_OBJDIR)
	$(CC) $(CCFLAGS) -I$(APP_INCDIR) -c $< -o $@

# Application asm sources
$(APP_OBJDIR)/%.o: $(APP_ASMDIR)/%.s | $(APP_OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

# Application 1 ELF
$(TARGET_DIR)/$(APP_1_TARGET).elf: $(APP_OBJS) $(LIB_OBJS) | $(TARGET_DIR)
ifeq ($(OUTPUT_MAPS), true)
	$(CC) $(LDFLAGS) -Xlinker -Map=$(TARGET_DIR)/$(APP_1_TARGET).map -T $(APP1_LDSCRIPT) $^ -o $@
else
	$(CC) $(LDFLAGS) -T $(APP1_LDSCRIPT) $^ -o $@
endif

# Application 1 binary
$(TARGET_DIR)/$(APP_1_TARGET).bin: $(TARGET_DIR)/$(APP_1_TARGET).elf | $(TARGET_DIR)
	$(OCPY) -O binary $< $@

# Application 1 HEX
$(TARGET_DIR)/$(APP_1_TARGET).hex: $(TARGET_DIR)/$(APP_1_TARGET).elf | $(TARGET_DIR)
	$(OCPY) -O ihex $< $@

# Application 2 ELF
$(TARGET_DIR)/$(APP_2_TARGET).elf: $(APP_OBJS) $(LIB_OBJS) | $(TARGET_DIR)
ifeq ($(OUTPUT_MAPS), true)
	$(CC) $(LDFLAGS) -Xlinker -Map=$(TARGET_DIR)/$(APP_2_TARGET).map -T $(APP2_LDSCRIPT) $^ -o $@
else
	$(CC) $(LDFLAGS) -T $(APP2_LDSCRIPT) $^ -o $@
endif

# Application 2 binary
$(TARGET_DIR)/$(APP_2_TARGET).bin: $(TARGET_DIR)/$(APP_2_TARGET).elf | $(TARGET_DIR)
	$(OCPY) -O binary $< $@

# Application 2 HEX
$(TARGET_DIR)/$(APP_2_TARGET).hex: $(TARGET_DIR)/$(APP_2_TARGET).elf | $(TARGET_DIR)
	$(OCPY) -O ihex $< $@


# === LIBRARY BUILD RULES ===
# Library sources
$(LIB_OBJS): $(LIB_SRCS) | $(LIB_OBJDIR)
	$(foreach lib,$(LIB_SRCS),$(CC) $(CCFLAGS) -c $(lib) -o $(LIB_OBJDIR)/$(notdir $(lib:%.c=%.o))$(\n))

clean_libs:
	rm -f $(LIB_OBJS)


# === DIRECTORY CREATION RULES ===
$(TARGET_DIR):
	mkdir -p $@

$(BL_OBJDIR):
	mkdir -p $@

$(APP_OBJDIR):
	mkdir -p $@

$(LIB_OBJDIR):
	mkdir -p $@