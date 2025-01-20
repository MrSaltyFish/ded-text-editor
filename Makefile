CC = gcc
SRC = main.c
OUT = te
CFLAGS = -Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags SDL2`
LIBS = `pkg-config --libs SDL2` -lm
BUILD_DIR = build
TEMP_SRC = temp.c
TEMP_OUT = temp

# Build the program
$(BUILD_DIR)/$(OUT): $(SRC)
	@mkdir -p $(BUILD_DIR)  # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(OUT) $(SRC) $(LIBS)

# Default target: Build and then run
all: $(BUILD_DIR)/$(OUT)
	# Run the compiled program
	./$(BUILD_DIR)/$(OUT)

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)

# Build target
build: $(BUILD_DIR)/$(OUT)
	@echo "Build successful"

temp:
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TEMP_OUT) $(TEMP_SRC) $(LIBS)

# Run the compiled program
run: $(BUILD_DIR)/$(OUT)
	./$(BUILD_DIR)/$(OUT)