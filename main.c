
/*
 *
 *   Ded (Text Editor) - Copied Project from Tsoding Daily -
 * https://www.youtube.com/watch?v=2UY_Am-Q-oI&list=PLpM-Dvs8t0VZVshbPeHPculzFFBdQWIFu
 *
 */

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "headers/font.h"
#include "headers/la.h"
#include "headers/sdllogger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

#define ALPHA(color) ((color) >> (8 * 3)) & 0xFF

#define UNHEXA(color)                                         \
	((color) >> (8 * 0)) & 0xFF, ((color) >> (8 * 1)) & 0xFF, \
		((color) >> (8 * 2)) & 0xFF

/*
 * SDL Check Code - Basically panics if the code is negative, as that is the
 * indication of a SDL error. It is a standard thing to do.
 *
 */
void scc(int code) {
	if (code < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL ERROR: %s\n", SDL_GetError());
		exit(1);
	}
}

/*
 * SDL Check Pointer - this accepts the pointer for the SDL functions that
 * return a pointer.
 *
 */
void *scp(void *ptr) {
	if (ptr == NULL) {
		fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL ERROR: %s\n", SDL_GetError());
		exit(1);
	}
	return ptr;
}

/*
 * Takes a png image and then extracts the characters from it into a SDL RGB
 * Surface.
 *
 */
SDL_Surface *surface_from_file(const char *file_path) {
	int width, height, n;

	unsigned char *pixels =
		stbi_load(file_path, &width, &height, &n, STBI_rgb_alpha);
	if (pixels == NULL) {
		fprintf(stderr, "Could not load file %s: %s\n", file_path,
				stbi_failure_reason());
		exit(1);
	}

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	const Uint32 rmask = 0xff000000;
	const Uint32 gmask = 0x00ff0000;
	const Uint32 bmask = 0x0000ff00;
	const Uint32 amask = 0x000000ff;
#else  // Little endian, like x86
	const Uint32 rmask = 0x000000ff;
	const Uint32 gmask = 0x0000ff00;
	const Uint32 bmask = 0x00ff0000;
	const Uint32 amask = 0xff000000;
#endif

	const int depth = 32;
	const int pitch = 4 * width;
	SDL_Surface *surface =
		scp(SDL_CreateRGBSurfaceFrom((void *)pixels, width, height, depth,
									 pitch, rmask, gmask, bmask, amask));

	if (!surface) {
		fprintf(stderr, "SDL_Surface creation failed: %s\n", SDL_GetError());
		stbi_image_free(pixels);  // Free the pixel data before exiting
		exit(1);
	}

	return surface;
}

/*
 * Font structure used for optimizing the speed by caching the texture
 * rectangles into an array so they are not calculated everytime.
 */
typedef struct {
	SDL_Texture *spritesheet;
	SDL_Rect glyph_table[ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1];
} Font;

/*
 * Loads the parameters of the Font structure
 */
Font font_load_from_file(SDL_Renderer *renderer, const char *file_path) {
	Font font = {0};

	SDL_Surface *font_surface = surface_from_file(file_path);
	scc(SDL_SetColorKey(font_surface, SDL_TRUE, 0xFF000000));
	font.spritesheet =
		scp(SDL_CreateTextureFromSurface(renderer, font_surface));

	SDL_FreeSurface(font_surface);

	for (size_t ascii = ASCII_DISPLAY_LOW; ascii <= ASCII_DISPLAY_HIGH;
		 ascii++) {
		const size_t index = ascii - ASCII_DISPLAY_LOW;	 // ASCII differnce
		const size_t col = index % FONT_COLS;
		const size_t row = index / FONT_COLS;

		font.glyph_table[index] = (SDL_Rect){
			.x = col * FONT_CHAR_WIDTH,
			.y = row * FONT_CHAR_HEIGHT,
			.w = FONT_CHAR_WIDTH,
			.h = FONT_CHAR_HEIGHT,
		};
	}

	return font;
}

/*
 * This function is for rendering characters at a given position.
 */
void render_char(SDL_Renderer *renderer, const Font *font, char c, Vec2f pos,
				 float scale) {
	SDL_Rect dst = {
		.x = (int)floorf(pos.x),
		.y = (int)floorf(pos.y),
		.w = (int)floorf(FONT_CHAR_WIDTH * scale),
		.h = (int)floorf(FONT_CHAR_HEIGHT * scale),
	};

	size_t index = '?' - ASCII_DISPLAY_LOW;

	if (ASCII_DISPLAY_LOW <= c && c <= ASCII_DISPLAY_HIGH) {
		index = c - ASCII_DISPLAY_LOW;	// ASCII differnce
	}

	// assert(c >= ASCII_DISPLAY_LOW);
	// assert(c <= ASCII_DISPLAY_HIGH);

	scc(SDL_RenderCopy(renderer, font->spritesheet, &font->glyph_table[index],
					   &dst));
}

void set_texture_color(SDL_Texture *texture, Uint32 color) {
	int r = (color >> (8 * 0)) & 0xff;
	int g = (color >> (8 * 1)) & 0xff;
	int b = (color >> (8 * 2)) & 0xff;
	int a = (color >> (8 * 3)) & 0xff;
	scc(SDL_SetTextureColorMod(texture, r, g, b));
	scc(SDL_SetTextureAlphaMod(texture, a));
}

void render_text_sized(SDL_Renderer *renderer, Font *font, const char *text,
					   size_t text_size, Vec2f pos, Uint32 color, float scale) {
	// Replaced by set_texture_color()
	// int r = (color >> (8 * 0)) & 0xff;
	// int g = (color >> (8 * 1)) & 0xff;
	// int b = (color >> (8 * 2)) & 0xff;
	// int a = (color >> (8 * 3)) & 0xff;

	// scc(SDL_SetTextureColorMod(font->spritesheet, r, g, b));
	// scc(SDL_SetTextureAlphaMod(font->spritesheet, a));

	set_texture_color(font->spritesheet, color);

	Vec2f pen = pos;
	for (size_t i = 0; i < text_size; i++) {
		render_char(renderer, font, text[i], pen, scale);
		pen.x += FONT_CHAR_WIDTH * scale;
	}
	// SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Text displayed.");
}

/*
 * This function is for rendering text passed into it.
 */
void render_text(SDL_Renderer *renderer, Font *font, const char *text,
				 Vec2f pos, Uint32 color, float scale) {
	render_text_sized(renderer, font, text, strlen(text), pos, color, scale);
}

/*
 * Input buffer
 */
#define BUFFER_CAPACITY 1024

char buffer[BUFFER_CAPACITY];
size_t buffer_size = 0;
size_t buffer_cursor = 0;

void buffer_insert_text_before_cursor(const char *text) {
	size_t text_size = strlen(text);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Text: %s", text);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Text size: %llu", text_size);
	const size_t free_space = BUFFER_CAPACITY - buffer_size;
	if (text_size > free_space) {
		text_size = free_space;
	}
	memmove(buffer + buffer_cursor + text_size, buffer + buffer_cursor,
			buffer_size - buffer_cursor);
	memcpy(buffer + buffer_cursor, text, text_size);
	buffer_size += text_size;
	buffer_cursor += text_size;
}

void buffer_backspace(void) {
	if (buffer_cursor > 0 && buffer_size > 0) {
		memmove(buffer + buffer_cursor - 1, buffer + buffer_cursor,
				buffer_size - buffer_cursor);
		buffer_size -= 1;
		buffer_cursor -= 1;
	}
}

void buffer_delete(void) {
	if (buffer_cursor < buffer_size && buffer_size > 0) {
		memmove(buffer + buffer_cursor, buffer + buffer_cursor + 1,
				buffer_size + buffer_cursor);
		buffer_size -= 1;
	}
}

/*
 * This is code to just display the cursor
 */

void render_cursor(SDL_Renderer *renderer, const Font *font) {
	Vec2f pos = vec2f(buffer_cursor * FONT_CHAR_WIDTH * FONT_SCALE, 0);
	const SDL_Rect rect = {
		.x = (int)floorf(pos.x),
		.y = (int)floorf(pos.y),
		.w = FONT_CHAR_WIDTH * FONT_SCALE,
		.h = FONT_CHAR_HEIGHT * FONT_SCALE,
	};

	// int r = (color >> (8 * 0)) & 0xff;
	// int g = (color >> (8 * 1)) & 0xff;
	// int b = (color >> (8 * 2)) & 0xff;
	// int a = (color >> (8 * 3)) & 0xff;

	int r = 0xFF;
	int g = 0xFF;
	int b = 0xFF;
	int a = 0xFF;

	scc(SDL_SetRenderDrawColor(renderer, r, g, b, a));
	scc(SDL_RenderFillRect(renderer, &rect));

	set_texture_color(font->spritesheet, 0xFF000000);

	if (buffer_cursor < buffer_size) {
		render_char(renderer, font, buffer[buffer_cursor], pos, FONT_SCALE);
	}
}

// TODO: Jump forward/backward by a word
// TODO: Delete a word
// TODO: move the cursor around
// TODO: multiple lines
// TODO: Save/Load file

int main1(int argc, char **argv) {
	buffer_insert_text_before_cursor("Hello, World");
	buffer_cursor = 5;
	buffer_insert_text_before_cursor("Foo, bar");
	char text[100] = {0};
	strcpy(text, buffer);
	int i;
	return 0;
}

/********************************
 *		MAIN Starts here		*
 ********************************/
int main(int argc, char **argv) {
	// Open a log file
	FILE *log_file = fopen("sdl_log.txt", "w");
	if (log_file == NULL) {
		fprintf(stderr, "Could not open log file for writing!\n");
		SDL_Quit();
		return 1;
	}
	// Set the custom log function
	SDL_LogSetOutputFunction(textLogger, log_file);

	// Log messages with different priorities
	// Note: Only some categories work with some Logs
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "This is an informational log.");
	SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "This is a warning log.");
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "This is an error log.");

	/********************************
	 *		SDL Starts here			*
	 ********************************/
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Starting Initialization...");

	scc(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window *window = scp(SDL_CreateWindow("Text Editor", 400, 400, 900, 800,
											  SDL_WINDOW_RESIZABLE));
	SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "SDL Window: \n");

	SDL_Renderer *renderer =
		scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

	Font font =
		font_load_from_file(renderer, "./fonts/charmap-oldschool_white.png");

	bool quit = false;
	while (!quit) {
		SDL_Event event = {0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					quit = true;
				} break;
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_BACKSPACE: {
							buffer_backspace();

						} break;
						case SDLK_DELETE: {
							buffer_delete();
						} break;
						case SDLK_LEFT: {
							if (buffer_cursor > 0) {
								buffer_cursor -= 1;
							}
						} break;
						case SDLK_RIGHT: {
							if (buffer_cursor < buffer_size) {
								buffer_cursor += 1;
							}
						} break;
						case SDLK_UP: {
						} break;
						case SDLK_DOWN: {
						} break;
					}
				} break;

				case SDL_TEXTINPUT: {
					SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
								"Received text input: %s", event.text.text);
					buffer_insert_text_before_cursor(event.text.text);
				} break;
			}

			scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
			scc(SDL_RenderClear(renderer));

			render_text_sized(renderer, &font, buffer, buffer_size,
							  vec2f(0.0, 0.0), 0xFFFFFFFF, FONT_SCALE);
			render_cursor(renderer, &font);

			SDL_RenderPresent(renderer);
			// SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL: Rendered!");
		}
	}

	// Clean up and quit SDL
	fclose(log_file);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}