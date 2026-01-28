#!/bin/bash

qemu-system-aarch64 -cpu cortex-a72 \
	-machine virt,gic-version=3,virtualization=on -m 512M -smp 1 \
	-nographic \
	-bios ./loader/u-boot.bin \
	-device loader,file=./build/UImage,addr=0x40200000,force-raw=on 
