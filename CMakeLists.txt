cmake_minimum_required(VERSION 3.20)
project(study-x-hyper LANGUAGES C ASM)

# Cross toolchain
set(CMAKE_C_COMPILER aarch64-none-elf-gcc)
set(CMAKE_ASM_COMPILER aarch64-none-elf-gcc)
set(CMAKE_LINKER aarch64-none-elf-ld)
set(CMAKE_OBJCOPY aarch64-none-elf-objcopy)
set(CMAKE_MKIMAGE mkimage)

# Disable system detection
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Compile flags
set(COMMON_FLAGS "-ffreestanding -fno-builtin -mgeneral-regs-only")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "-D__ASSEMBLY__")

# Source files
set(SOURCES
    head.S
    drivers/pl011.c
    utils/utils.c
    utils/printf.c
    arch/spinlock.c
    utils/xmalloc.c
    main.c
)

# Executable target
add_executable(image.elf ${SOURCES})

# Header files path
target_include_directories(image.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/arch
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/utils
)

# Linker scripts
set_target_properties(image.elf PROPERTIES
    LINKER_LANGUAGE C
    LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/linker.ld"
)

# Linker command
set(CMAKE_C_LINK_EXECUTABLE
    "${CMAKE_LINKER} <LINK_FLAGS> -o <TARGET> <OBJECTS>"
)
set(CMAKE_ASM_LINK_EXECUTABLE ${CMAKE_C_LINK_EXECUTABLE})

# Remove ELF information
add_custom_command(
    OUTPUT image.bin
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:image.elf> image.bin
    DEPENDS image.elf
    COMMENT "Converting ELF to raw binary: image.bin"
)

# Create UImage
add_custom_target(UImage
    COMMAND ${CMAKE_MKIMAGE}
        -A arm64 -O linux -C none
        -a 0x40200000 -e 0x40200000
        -d image.bin UImage
    DEPENDS image.bin
    COMMENT "Creating U-Boot UImage"
)

add_custom_target(all_images ALL DEPENDS UImage)

# Run target
find_program(QEMU_SYSTEM_AARCH64 qemu-system-aarch64)
if(QEMU_SYSTEM_AARCH64 AND EXISTS "${CMAKE_SOURCE_DIR}/scripts/run_qemu.sh")
    add_custom_target(run
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/run_qemu.sh
        DEPENDS UImage
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Launching QEMU"
    )
endif()
