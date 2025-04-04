CC = gcc
SRC = main.c
DEPENDS = la.c editor.c
OUT = te
CFLAGS = -Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags SDL2`
LIBS = `pkg-config --libs SDL2` -lm
BUILD_DIR = build

# Build the program
$(BUILD_DIR)/$(OUT): $(SRC)
	@mkdir -p $(BUILD_DIR)  # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(OUT) $(SRC) $(DEPENDS) $(LIBS)

# Default target: Build and then run
all: $(BUILD_DIR)/$(OUT)
	# Run the compiled program
	./$(BUILD_DIR)/$(OUT)

# Clean up the build directory and all compiled files
clean:
	rm -rf $(BUILD_DIR) $(TEMP_OUT)

# Build target
build: $(BUILD_DIR)/$(OUT)

# Run the compiled program
run: $(BUILD_DIR)/$(OUT)
	./$(BUILD_DIR)/$(OUT)

debug: $(BUILD_DIR)/$(OUT)
	gdb ./$(BUILD_DIR)/$(OUT)