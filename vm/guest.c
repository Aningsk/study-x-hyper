#include "types.h"
#include "guest.h"

/* GuestVM image are packaged in X-Hyper image with aarch64-none-elf-ld */
extern char _binary_GuestVM_start[];
extern char _binary_GuestVM_end[];
extern char _binary_GuestVM_size[];

guest_t guest_vm_image = {
	.guest_name = "guest_vm",
	.start_addr  = (u64)_binary_GuestVM_start,
	.image_size  = (u64)_binary_GuestVM_size,
};
