# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := photon.elf
 
# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.

CC := x86_64-elf-gcc

 
# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".

LD := x86_64-elf-ld

 
# User controllable CFLAGS.
CFLAGS ?= -O2 -g -Wall -Wextra -Wpedantic -pipe
 
# User controllable linker flags. We set none by default.
LDFLAGS ?=
 
# Internal C flags that should not be changed by the user.
override INTERNALCFLAGS :=   \
	-I.                  \
	-std=c11             \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-stack-check     \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD				 \
	-w 					 
 
# Internal linker flags that should not be changed by the user.
override INTERNALLDFLAGS :=    \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static
 
# Use find to glob all *.c files in the directory and extract the object names.
override CFILES := $(shell find ./ -type f -name '*.c')
override OBJ := $(CFILES:.c=.o)
override HEADER_DEPS := $(CFILES:.c=.d)
 
# Default target.
.PHONY: all
all: $(KERNEL)
 
# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@
 
# Compilation rules for *.c files.
-include $(HEADER_DEPS)
%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@
.PHONY: everything
everything:
	@echo "---OS BUILD START!---"
	@echo "---BUILDING THE KERNEL---"
	@make
	@echo "---KERNEL BUILD COMPLETE---"
	@echo "---PERFORMING STAGE2---"
	@make stage2
	@echo "---STAGE2 COMPLETE---"
	@echo "OS BUILT WITH NO ERRORS!"
	@echo "---OS BUILD COMPLETE---"
.PHONY: run
run:
	@echo "---RUNNING---"
	@qemu-system-x86_64 image.iso -serial stdio -enable-kvm -m 3G -smp 2
	@echo "---RUNNING FINISHED---"
# Remove object files and the final executable.
.PHONY: clean
clean:
ifeq "$(wildcard $(KERNEL))" "$(KERNEL)"
	@echo "Removing Kernel and object files..."
	@rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)
	@echo "Done!"
else
	@echo "No kernel or object files to clean, Skipping..."
endif
ifneq "$(wildcard *.iso)" ""
	@echo "Removing image.iso"
	@rm -rf *.iso
	@echo "Done!"
else
	@echo "No image to clean, Skipping..."
endif
ifeq "$(wildcard limine)" "limine"
	@echo "Removing the Limine Folder"
	@rm -rf limine
	@echo "Done!"
else
	@echo "No limine folder to clean, Skipping..."
endif
ifeq "$(wildcard iso_root)" "iso_root"
	@echo "Removing the iso_root Folder"
	@rm -rf iso_root
	@echo "Done!"
else
	@echo "No iso_root folder to clean, Skipping..."
endif
	@echo "---LEFTOVER CLEANUP---"
ifneq "$(wildcard *.d)" ""
	@echo "Removing .d files"
	@rm -rf *.d
	@echo "Done!"
else
	@echo "No .d files to clean, Skipping..."
endif
ifneq "$(wildcard *.o)" ""
	@echo "Removing .o files"
	@rm -rf *.o
	@echo "Done!"
else
	@echo "No .o files to clean, Skipping..."
endif
	@echo "---LEFTOVER CLEANUP---"
stage2:
	@echo "---STAGE 2---"
	@./stage2.sh
	@echo "---STAGE 2---"