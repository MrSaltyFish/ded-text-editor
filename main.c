
/*
 *
 *   Ded (Text Editor) - Copied Project from Tsoding Daily -
 * https://www.youtube.com/watch?v=2UY_Am-Q-oI&list=PLpM-Dvs8t0VZVshbPeHPculzFFBdQWIFu
 *	// Haha merge conflict go brr
 */

// Hello from te

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
#include "./headers/sv.h"
#include "headers/editor.h"
#include "headers/stb_image.h"

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

#define ALPHA(color) ((color) >> (8 * 3)) & 0xFF

#define UNHEXA(color)                                         \
	((color) >> (8 * 0)) & 0xFF, ((color) >> (8 * 1)) & 0xFF, \
		((color) >> (8 * 2)) & 0xFF

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 144
#define DELTA_TIME (1.0f / FPS)
#define CAMERA_SPEED 5.0f

/*
 * SDL Check Code - Basically panics if the code is negative, as that is the
 * indication of a SDL error. It is a standard thing to do.
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

	size_t index = c - ASCII_DISPLAY_LOW;  // ASCII differnce

	// size_t index = '?' - ASCII_DISPLAY_LOW;

	// if (ASCII_DISPLAY_LOW <= c && c <= ASCII_DISPLAY_HIGH) {
	// 	index = c - ASCII_DISPLAY_LOW;	// ASCII differnce
	// }

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
	set_texture_color(font->spritesheet, color);

	Vec2f pen = pos;
	for (size_t i = 0; i < text_size; i++) {
		render_char(renderer, font, text[i], pen, scale);
		pen.x += FONT_CHAR_WIDTH * scale;
	}
}

/*
 * This function is for rendering text passed into it.
 */
void render_text(SDL_Renderer *renderer, Font *font, const char *text,
				 Vec2f pos, Uint32 color, float scale) {
	render_text_sized(renderer, font, text, strlen(text), pos, color, scale);
}

/*
 * Input line->chars
 */
// Line line = {0};
// size_t cursor = 0;

// Editor
Editor editor = {0};
Vec2f camera_pos = {0};
Vec2f camera_vel = {0};
/*
 * This is code to just display the cursor
 */

Vec2f window_size(SDL_Window *window) {
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	return vec2f((float)w, (float)h);
}

Vec2f camera_project_point(SDL_Window *window, Vec2f point) {
	return vec2f_add(vec2f_sub(point, camera_pos),
					 vec2f_mul(window_size(window), vec2fs(0.5f)));
}

void render_cursor(SDL_Renderer *renderer, SDL_Window *window,
				   const Font *font) {
	const Vec2f pos = camera_project_point(
		window,
		vec2f((float)editor.cursor_col * FONT_CHAR_WIDTH * FONT_SCALE,
			  (float)editor.cursor_row * FONT_CHAR_HEIGHT * FONT_SCALE));

	const SDL_Rect rect = {
		.x = (int)floorf(pos.x),
		.y = (int)floorf(pos.y),
		.w = FONT_CHAR_WIDTH * FONT_SCALE,
		.h = FONT_CHAR_HEIGHT * FONT_SCALE,
	};

	Uint32 color = 0xFFFFFFFF;
	int r = (color >> (8 * 0)) & 0xff;
	int g = (color >> (8 * 1)) & 0xff;
	int b = (color >> (8 * 2)) & 0xff;
	int a = (color >> (8 * 3)) & 0xff;

	scc(SDL_SetRenderDrawColor(renderer, r, g, b, a));
	scc(SDL_RenderFillRect(renderer, &rect));

	const char *c = render_char_under_cursor(&editor);
	if (c) {
		set_texture_color(font->spritesheet, 0xFF000000);
		render_char(renderer, font, *c, pos, FONT_SCALE);
	}
}

void usage(FILE *stream) { fprintf(stream, "Usage: te [FILE-PATH]\n"); }

// TODO: Delete a line
// TODO: Split line on enter
// TODO: ncurses
// TODO: Save File
// TODO: Jump forward/backward by a word
// TODO: Delete a word
// TODO: Blinking cursor

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

	(void)argc;
	(void)argv;
	const char *file_path = NULL;

	if (argc > 1) {
		file_path = argv[1];
	}

	if (file_path) {
		FILE *file = fopen(file_path, "r");
		if (file != NULL) {
			editor_load_from_file(&editor, file);
			fclose(file);
		}
	}

	scc(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window *window =
		scp(SDL_CreateWindow("Text Editor", 200, 400, SCREEN_WIDTH,
							 SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
				"SDL Window: Text Editor Created!\n");

	SDL_Renderer *renderer =
		scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
				"SDL Window: SDL_CreateRenderer Created!\n");
	Font font =
		font_load_from_file(renderer, "./fonts/charmap-oldschool_white.png");
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
				"SDL Window: font_load_from_file Created!\n");
	bool quit = false;
	int camera_speed = CAMERA_SPEED;
	while (!quit) {
		const Uint32 start = SDL_GetTicks();
		SDL_Event event = {0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					quit = true;
				} break;
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_BACKSPACE: {
							editor_backspace(&editor);

						} break;
						case SDLK_F2: {
							if (file_path) {
								editor_save_to_file(&editor, file_path);
							}

							editor_save_to_file(&editor, "output");
						} break;
						// case SDLK_F5: {
						// 	if (camera_speed < 3000) {
						// 		camera_speed += 10;
						// 		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
						// 					"Camera Speed Increased:  %d",
						// 					camera_speed);
						// 	}
						// } break;
						// case SDLK_F6: {
						// 	if (camera_speed > 6) {
						// 		camera_speed -= 5;
						// 		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
						// 					"Camera Speed Reduced:  %d",
						// 					camera_speed);
						// 	}
						// } break;
						// case SDLK_F7: {
						// 	if (camera_speed > 6) {
						// 		camera_speed = 0;
						// 		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
						// 					"Camera Speed :  0");
						// 	}
						// } break;
						case SDLK_RETURN:
						case SDLK_RETURN2: {
							editor_insert_new_line(&editor);
						} break;
						case SDLK_DELETE: {
							editor_delete(&editor);
						} break;
						case SDLK_LEFT: {
							if (editor.cursor_col > 0) {
								editor.cursor_col -= 1;
							}
						} break;
						case SDLK_RIGHT: {
							editor.cursor_col += 1;

							// if (editor.cursor_col <
							// 	editor.lines[editor.cursor_row].size) {
							// 	editor.cursor_col += 1;
							// }
						} break;
						case SDLK_UP: {
							if (editor.cursor_row > 0) {
								editor.cursor_row -= 1;
							}
						} break;
						case SDLK_DOWN: {
							editor.cursor_row += 1;
							// if (editor.cursor_row < editor.size) {
							// 	editor.cursor_row += 1;
							// }
						} break;
					}
				} break;

				case SDL_TEXTINPUT: {
					// SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
					// 			"Received text input: %s", event.text.text);
					editor_insert_text_before_cursor(&editor, event.text.text);
				} break;
			}
		}
		const Vec2f cursor_pos =
			vec2f((float)editor.cursor_col * FONT_CHAR_WIDTH * FONT_SCALE,
				  (float)editor.cursor_row * FONT_CHAR_HEIGHT * FONT_SCALE);
		camera_vel =
			vec2f_mul(vec2f_sub(cursor_pos, camera_pos), vec2fs(camera_speed));
		camera_pos =
			vec2f_add(camera_pos, vec2f_mul(camera_vel, vec2fs(DELTA_TIME)));

		// SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
		// 			"Camera position: (%f, %f)\n", camera_vel.x,
		// 			camera_vel.y);

		scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
		scc(SDL_RenderClear(renderer));

		for (size_t row = 0; row < editor.size; ++row) {
			const Line *line = editor.lines + row;
			const Vec2f line_pos = camera_project_point(
				window,
				vec2f(0.0f, (float)row * FONT_CHAR_HEIGHT * FONT_SCALE));
			render_text_sized(renderer, &font, line->chars, line->size,
							  line_pos, 0xFFFFFFFF, FONT_SCALE);
		}
		render_cursor(renderer, window, &font);

		SDL_RenderPresent(renderer);

		const Uint32 duration = SDL_GetTicks() - start;
		const Uint32 delta_time_ms = 1000 / FPS;
		// SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
		// 			"Rendered frame, duration: %d", duration);

		if (duration < delta_time_ms) {
			SDL_Delay(delta_time_ms - duration);
		}
	}

	// Clean up and quit SDL
	fclose(log_file);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
