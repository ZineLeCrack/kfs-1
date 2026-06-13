# ===== TOOLCHAIN =====
TARGET := i686-elf

AS := $(TARGET)-as
CC := $(TARGET)-gcc

# ===== FLAGS =====
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs # -fno-rtti
LDFLAGS := -T srcs/linker.ld -ffreestanding -O2 -nostdlib

# ===== SOURCES =====
ASM_SRCS := boot.s
C_SRCS := utils.c printk.c input_handler.c windows_manager.c kernel.c

SRCS_DIR := srcs
OBJS_DIR := objs

OBJS := $(ASM_SRCS:%.s=$(OBJS_DIR)/%.o) $(C_SRCS:%.c=$(OBJS_DIR)/%.o)

# ===== OUTPUT =====
KERNEL := myos

# ===== BUILD KERNEL =====
all: $(OBJS_DIR) $(KERNEL)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.s
	$(AS) $< -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) -Iincludes

$(KERNEL): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) -lgcc

# ===== RUN =====
run:
	qemu-system-i386 -kernel $(KERNEL)

# ===== CLEAN =====
clean:
	rm -rf $(OBJS_DIR)

# ===== FCLEAN =====
fclean: clean
	rm -f $(KERNEL)

# ===== RE =====
re: fclean all

.PHONY: run clean fclean re
