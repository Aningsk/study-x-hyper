# Study X-Hyper
This repo is just for the study of X-Hyper.  
X-Hyper URL: https://gitcode.com/cxjczy1990/X-Hyper 

## How to build
```
cd study-x-hyper/
mkdir build
cd build
cmake ..
```
To build only: `make` 
To build and run: `make run`
To build and debug (GDB on TCP 1991): `make debug`
To clean: `make clean` 

**Notice**:  
study-x-hyper depends on u-boot to boot up.  
Please make sure you have u-boot source code, as
```
➜  Hypervisor tree -L 1
.
├── study-x-hyper
└── u-boot-2025.10

2 directories, 0 files
```
Under `study-x-hyper/loader/` , the `u-boot.bin` should link to:   
`../../u-boot-2025.10/u-boot.bin`   

The u-boot should be built successfully with this build scripts to create `u-boot.bin` :  
```
#!/bin/bash

export ARCH=arm64
export CROSS_COMPILE=aarch64-none-elf-

make qemu_arm64_defconfig
sed -i 's/CONFIG_PREBOOT="usb start"/CONFIG_PREBOOT="bootm 0x40200000 - ${fdtcontroladdr}"/g' "./.config"

make -j24
```
