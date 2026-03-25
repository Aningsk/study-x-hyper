#!/bin/bash
set -e

echo ">>>> Building minimal GuestVM"

# Clean up previous builds
rm -f GuestVM.elf GuestVM GuestVM.o

# 1. Build GuestVM.elf
make

# 2. Get pure binary from GuestVM.elf
aarch64-none-elf-objcopy -O binary GuestVM.elf GuestVM

# 3. transform raw binary to object file with symbols
# This step is necessary to include the binary in the final object file,
# as it will contain the symbols _binary_GuestVM_start and _binary_GuestVM_size.
# These symbols will be used to access the binary data in the GuestVM.
aarch64-none-elf-ld -r -b binary GuestVM -o GuestVM.o

# 4. Clean up intermediate files and *.o
rm -f GuestVM.elf GuestVM
make clean

echo ">>>> Built GuestVM.o with symbols: _binary_GuestVM_start, _binary_GuestVM_size"
