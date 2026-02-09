#!/bin/bash
set -e

echo ">>>> Building minimal GuestVM"

# Clean up previous builds
rm -f GuestVM.elf GuestVM GuestVM.o

# 1. head.S -> head.o
aarch64-none-elf-gcc -Wall -nostdlib -mgeneral-regs-only \
    -c head.S -o head.o

# 2. Link GuestVM.elf
aarch64-none-elf-ld -T linker.ld head.o -o GuestVM.elf

# 3. Get pure binary from GuestVM.elf
aarch64-none-elf-objcopy -O binary GuestVM.elf GuestVM

# 4. transform raw binary to object file with symbols
# This step is necessary to include the binary in the final object file,
# as it will contain the symbols _binary_GuestVM_start and _binary_GuestVM_size.
# These symbols will be used to access the binary data in the GuestVM.
aarch64-none-elf-ld -r -b binary GuestVM -o GuestVM.o

# 5. Clean up intermediate files
rm -f head.o GuestVM.elf GuestVM

echo ">>>> Built GuestVM.o with symbols: _binary_GuestVM_start, _binary_GuestVM_size"
