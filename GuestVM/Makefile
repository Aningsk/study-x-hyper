# Toolchain
CC      := aarch64-none-elf-gcc
LD      := aarch64-none-elf-ld
OBJCOPY := aarch64-none-elf-objcopy

# Flags
CFLAGS  := -Wall -O2 -ffreestanding -fno-stack-protector -mgeneral-regs-only
ASFLAGS := -Wall -mgeneral-regs-only
LDFLAGS := -T linker.ld

# Source files
ASM_SOURCES := head.S
C_SOURCES   := main.c pl011.c printf.c utils.c

# Object files
OBJECTS := $(ASM_SOURCES:.S=.o) $(C_SOURCES:.c=.o)

# Target
TARGET_ELF := GuestVM.elf

.PHONY: all clean

all: $(TARGET_ELF)

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET_ELF)
