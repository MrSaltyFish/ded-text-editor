CC = gcc
SRC = main.c
OUT = te
CFLAGS = -Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags SDL2`
LIBS = `pkg-config --libs SDL2` -lm
BUILD_DIR = build

# Build the program
$(BUILD_DIR)/$(OUT): $(SRC)
	@mkdir -p $(BUILD_DIR)  # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(OUT) $(SRC) $(LIBS)

# Default target: Build and then run
all: $(BUILD_DIR)/$(OUT)
	# Run the compiled program
	@if [ "$(OS)" = "Windows_NT" ]; then \
		$(BUILD_DIR)/$(OUT).exe; \
	else \
		$(BUILD_DIR)/$(OUT); \
	fi

clean:
	rm -rf $(BUILD_DIR)
