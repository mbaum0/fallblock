SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HEADER_FILES = $(wildcard $(HEADERS_DIR)/*.h)
INCLUDE_PATHS = -Iinclude/

OBJ_NAME = fallblock
BUILD_TARGET = $(BUILD_DIR)/$(OBJ_NAME)

COMPILER_FLAGS = -std=c11 -o0 -Wall -Wextra -Wpedantic -Werror -g -gdwarf-4
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm
OTHER_FLAGS = -DLOG_USE_COLOR

CC = clang

build: $(BUILD_TARGET)

format:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*.c
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i include/*.h


$(BUILD_TARGET): $(SRC_FILES) $(HEADER_FILES)
	mkdir -p $(BUILD_DIR)
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(OTHER_FLAGS) $(INCLUDE_PATHS) $(SRC_FILES) -o $(BUILD_TARGET)

clean:
	rm -rf $(BUILD_DIR)

suppress: $(BUILD_TARGET)
	rm -f sdl.supp memcheck.log
	valgrind --leak-check=full --show-leak-kinds=all --gen-suppressions=all --log-file=memcheck.log ./build/fallblock
	cat memcheck.log | ./s.sh > sdl.supp

leaks: $(BUILD_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=sdl.supp ./$(BUILD_TARGET)