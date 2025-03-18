CC = gcc
SRCS=src/main.c src/la.c src/editor.c src/font.c src/sdl_extra.c src/file.c src/gl_extra.c
OUT = te
# GLEW headers # GLEW headers # SDL2 flags

PKGS = sdl2 glew
CFLAGS = -Wall -Wextra -std=c11 -pedantic -ggdb \
    -IC:/msys64/mingw64/include \
    -IC:/msys64/mingw64/include/GL \
    `pkg-config --cflags $(PKGS)`

# GLEW library path # Link to GLEW
LIBS = `pkg-config --libs sdl2 glew` -lm -LC:/msys64/mingw64/lib -lglew32 -lopengl32

BUILD_DIR = build

# Build the program
$(BUILD_DIR)/$(OUT): $(SRCS)
	@mkdir -p $(BUILD_DIR)  # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(OUT) $(SRCS) $(LIBS)

# Default target: Build and then run
all: $(BUILD_DIR)/$(OUT)
	# Run the compiled program
	./$(BUILD_DIR)/$(OUT)

# Clean up the build directory and all compiled files
clean:
	rm -rf $(BUILD_DIR)

# Run the compiled program
run: $(BUILD_DIR)/$(OUT)
	./$(BUILD_DIR)/$(OUT)

debug: $(BUILD_DIR)/$(OUT)
	gdb -tui ./$(BUILD_DIR)/$(OUT)

te: $(SRCS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/te $(SRCS) $(LIBS)