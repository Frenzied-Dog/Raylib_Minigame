# ==============================================================================
# 1. 專案路徑變數 (Project Path Variables)
# ------------------------------------------------------------------------------
SRC_FILES := src/*.c
INC_PATH  := include
BIN_DIR   := build/debug
REL_DIR   := build/release

# raylib 庫路徑 (與 tasks.json 一致)
LIB_PATH_WIN64  := lib/win64
LIB_PATH_MAC    := lib/mac
LIB_PATH_LINUX  := lib/linux

# 預設為 Debug 構建
BUILD_TYPE := debug

# ==============================================================================
# 2. 構建類型通用變數 (Build Type Common Variables)
# ------------------------------------------------------------------------------

# 通用編譯標誌 (來自 tasks.json)
debug_CFLAGS := -g -fdiagnostics-color=always -I"$(INC_PATH)"
release_CFLAGS := -O2 -fdiagnostics-color=always -I"$(INC_PATH)"

# ==============================================================================
# 3. 平台和構建類型組合變數 (Combined Platform & Build Variables)
#    此部分合併了平台檢測、編譯器選取與函式庫鏈接設定。
# ------------------------------------------------------------------------------

# 3.1 Windows (win64)
win64_CC := C:/MinGW64/bin/gcc.exe
win64_LIB_PATH := $(LIB_PATH_WIN64)
win64_TARGET := main.exe
win64_LIBS := -lraylib -lgdi32 -lwinmm

# 3.2 macOS (mac)
mac_CC := /usr/bin/clang
mac_LIB_PATH := $(LIB_PATH_MAC)
mac_TARGET := main_mac
mac_LIBS := -lraylib -framework OpenGL -framework OpenAL -framework Cocoa -framework IOKit


# 3.3 Linux (linux)
linux_CC := /usr/bin/gcc
linux_LIB_PATH := $(LIB_PATH_LINUX)
linux_TARGET := main_linux
linux_LIBS := -lraylib -lGL -lm -ldl -lrt -lpthread -lX11

# ==============================================================================
# 4. 平台檢測與變數賦值 (Platform Detection and Variable Assignment)
# ------------------------------------------------------------------------------

ifeq ($(OS), Windows_NT)
	PLATFORM := win64
else
	OS := $(shell uname -s)
endif

# 檢測並設定 PLATFORM 變數
ifeq ($(OS), Darwin)
    PLATFORM := mac
endif

ifeq ($(OS), Linux)
    PLATFORM := linux
endif

ifndef PLATFORM
    $(error Unknown platform: $(OS). Please check the Makefile or run on Windows/macOS/Linux.)
endif

# 根據 PLATFORM 和 BUILD_TYPE 動態選擇變數
CC := $($(PLATFORM)_CC)
LIB_DIR := $($(PLATFORM)_LIB_PATH)
TARGET_NAME := $($(PLATFORM)_TARGET)
LIBS := $($(PLATFORM)_LIBS)
CFLAGS := $($(BUILD_TYPE)_CFLAGS)
OUT_DIR := $(if $(filter debug,$(BUILD_TYPE)), $(BIN_DIR), $(REL_DIR))

# ==============================================================================
# 5. 主要目標 (Main Targets)
# ------------------------------------------------------------------------------
.PHONY: all build debug release clean run

# 預設目標：執行 Debug 構建
all: debug

# 主要構建步驟
$(OUT_DIR)/$(TARGET_NAME): $(SRC_FILES)
	@echo "--- Building $(BUILD_TYPE) version for $(PLATFORM) using $(CC) ---"
# 	@mkdir -p $(OUT_DIR)
    # 編譯指令: [Compiler] [Flags] [Source Files] -o [Output] -L[Lib Path] [Libs]
	$(CC) $(CFLAGS) $(SRC_FILES) -o $@ -L"$(LIB_DIR)" $(LIBS)

# 執行構建
build: $(OUT_DIR)/$(TARGET_NAME)

# 設置 BUILD_TYPE 為 debug 並調用 build
debug:
	$(MAKE) build BUILD_TYPE=debug

# 設置 BUILD_TYPE 為 release 並調用 build
release:
	$(MAKE) build BUILD_TYPE=release

# 執行編譯後的程式
run: build
	@echo "--- Running $(TARGET_NAME) ---"
	./$(OUT_DIR)/$(TARGET_NAME)

# 清理目標
clean:
	@echo "--- Cleaning build directories ---"
	rm -f $(BIN_DIR)/* $(REL_DIR)/*
	rmdir $(BIN_DIR) 2>/dev/null || true
	rmdir $(REL_DIR) 2>/dev/null || true