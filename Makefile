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

# ===== BUILD KERNEL =====
all: $(KERNEL)

boot.o: $(ASM)
	$(AS) $< -o $@

kernel.o: $(C_SRC)
	$(CC) -c $< -o $@ $(CFLAGS)

$(KERNEL): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) -lgcc

# ===== RUN =====
run:
	qemu-system-i386 -kernel $(KERNEL)

# ===== CLEAN =====
clean:
	rm -f $(OBJ)
	rm -rf $(ISO_DIR)

# ===== FCLEAN =====
fclean: clean
	rm -f $(KERNEL)
