# ===== TOOLCHAIN =====
# export PATH="/goinfre/rlebaill/opt/cross/bin:$PATH"

PREFIX = /goinfre/rlebaill/opt/cross
TARGET = i686-elf

AS = $(TARGET)-as
CC = $(TARGET)-gcc

# ===== FLAGS =====
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs # -fno-rtti
LDFLAGS = -T srcs/linker.ld -ffreestanding -O2 -nostdlib

# ===== SOURCES =====
ASM = srcs/boot.s
C_SRC = srcs/kernel.c

OBJ = boot.o kernel.o

# ===== OUTPUT =====
KERNEL = myos
ISO_DIR = isodir
ISO = myos.iso

# ===== BUILD KERNEL =====
all: $(KERNEL) iso

boot.o: $(ASM)
	$(AS) $< -o $@

kernel.o: $(C_SRC)
	$(CC) -c $< -o $@ $(CFLAGS)

$(KERNEL): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) -lgcc

# ===== ISO =====
iso: $(KERNEL)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/$(KERNEL)
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

# ===== RUN =====
run:
	qemu-system-i386 -cdrom $(ISO)

# ===== CLEAN =====
clean:
	rm -f $(OBJ) $(KERNEL)
	rm -rf $(ISO_DIR)

# ===== FCLEAN =====
fclean: clean
	rm -f $(ISO)
