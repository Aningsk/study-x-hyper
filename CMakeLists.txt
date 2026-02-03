cmake_minimum_required(VERSION 3.20)
project(study-x-hyper LANGUAGES C ASM)

# -----------------------------
# Cross-compilation toolchain
# -----------------------------
set(CMAKE_C_COMPILER aarch64-none-elf-gcc)
set(CMAKE_ASM_COMPILER aarch64-none-elf-gcc)
set(CMAKE_LINKER aarch64-none-elf-ld)
set(CMAKE_OBJCOPY aarch64-none-elf-objcopy)
set(CMAKE_MKIMAGE mkimage)

# Disable system detection (bare-metal)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# -----------------------------
# Compile flags
# -----------------------------
set(COMMON_FLAGS "-ffreestanding -fno-builtin -mgeneral-regs-only")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "-D__ASSEMBLY__")

# -----------------------------
# Source files
# -----------------------------
set(SOURCES
    head.S
    drivers/pl011.c
    utils/utils.c
    utils/printf.c
    arch/spinlock.c
    utils/xmalloc.c
    utils/kalloc.c
    arch/mm.c
    main.c
)

# -----------------------------
# Executable target
# -----------------------------
add_executable(image.elf ${SOURCES})

target_include_directories(image.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/arch
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/utils
)

# -----------------------------
# Preprocess linker script
# -----------------------------
set(LINKER_SCRIPT_INPUT ${CMAKE_SOURCE_DIR}/linker.ld)
set(LINKER_SCRIPT_PROCESSED ${CMAKE_BINARY_DIR}/linker_processed.ld)

add_custom_command(
    OUTPUT ${LINKER_SCRIPT_PROCESSED}
    COMMAND ${CMAKE_C_COMPILER}
        -I${CMAKE_SOURCE_DIR}/arch
        -E -P -x c ${LINKER_SCRIPT_INPUT}
        -o ${LINKER_SCRIPT_PROCESSED}
    DEPENDS ${LINKER_SCRIPT_INPUT} ${CMAKE_SOURCE_DIR}/arch/layout.h
    COMMENT "Preprocessing linker script with CPP"
)

add_custom_target(linker_script ALL DEPENDS ${LINKER_SCRIPT_PROCESSED})

# Ensure the ELF target depends on the preprocessed linker script
add_dependencies(image.elf linker_script)

# -----------------------------
# Linking
# -----------------------------
# Override link command to use ld directly (as originally intended)
set(CMAKE_C_LINK_EXECUTABLE
    "${CMAKE_LINKER} <LINK_FLAGS> -o <TARGET> <OBJECTS>"
)
set(CMAKE_ASM_LINK_EXECUTABLE ${CMAKE_C_LINK_EXECUTABLE})

set_target_properties(image.elf PROPERTIES
    LINKER_LANGUAGE C
    LINK_FLAGS "-T ${LINKER_SCRIPT_PROCESSED}"
)

# -----------------------------
# Post-build: ELF => BIN => UImage
# -----------------------------
add_custom_command(
    OUTPUT image.bin
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:image.elf> image.bin
    DEPENDS image.elf
    COMMENT "Converting ELF to raw binary: image.bin"
)

add_custom_target(bin_image ALL DEPENDS image.bin)

add_custom_target(UImage
    COMMAND ${CMAKE_MKIMAGE}
        -A arm64 -O linux -C none
        -a 0x40200000 -e 0x40200000
        -d image.bin UImage
    DEPENDS image.bin
    COMMENT "Creating U-Boot UImage"
)

add_custom_target(all_images ALL DEPENDS UImage)

# -----------------------------
# Run in QEMU
# -----------------------------
find_program(QEMU_SYSTEM_AARCH64 qemu-system-aarch64)
if(QEMU_SYSTEM_AARCH64 AND EXISTS "${CMAKE_SOURCE_DIR}/scripts/run_qemu.sh")
    add_custom_target(run
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/run_qemu.sh
        DEPENDS UImage
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Launching QEMU"
    )
endif()

# -----------------------------
# Cleanup
# -----------------------------
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    ${LINKER_SCRIPT_PROCESSED}
    image.bin
    UImage
)
