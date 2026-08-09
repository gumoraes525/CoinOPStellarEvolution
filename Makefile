ZCC ?= zcc
TARGET ?= +embedded
CPU ?= -clib=sdcc_iy
CFLAGS ?= -O2 --max-allocs-per-node200000
BUILD_DIR := build
NAME := stellar

C_SOURCES := src/main.c src/game.c
ASM_SOURCES := src/pacman_hw.asm
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
           $(patsubst src/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

.PHONY: all clean

all: $(BUILD_DIR)/$(NAME).bin

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(ZCC) $(TARGET) $(CPU) $(CFLAGS) -Isrc -c $< -o $@

$(BUILD_DIR)/%.o: src/%.asm | $(BUILD_DIR)
	$(ZCC) $(TARGET) $(CPU) -c $< -o $@

$(BUILD_DIR)/$(NAME).bin: $(OBJECTS)
	$(ZCC) $(TARGET) $(CPU) -create-app -o $(BUILD_DIR)/$(NAME) $(OBJECTS)
	cp $(BUILD_DIR)/$(NAME)_CODE.bin $@

clean:
	rm -rf $(BUILD_DIR)
