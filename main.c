
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

#include "headers/font.h"
#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"

/*
 * Custom log function that writes logs to a file
 */
void my_log_function(void *userdata, int category, SDL_LogPriority priority,
					 const char *message) {
	FILE *log_file = (FILE *)userdata;
	fprintf(log_file, "SDL LOG [%d]: %s\n", priority, message);
}

/*
 * SDL Check Code - Basically panics if the code is negative, as that is the
 * indication of a SDL error. It is a standard thing to do.
 *
 */
void scc(int code) {
	if (code < 0) {
		fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
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
		exit(1);
	}
	return ptr;
}

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

int main(int argc, char **argv) {
	SDL_Log("Starting Initialization...\n");

	// Open a log file
	FILE *log_file = fopen("sdl_log.txt", "w");
	if (log_file == NULL) {
		fprintf(stderr, "Could not open log file for writing!\n");
		SDL_Quit();
		return 1;
	}
	// Log messages with different priorities
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "This is an informational log.");
	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "This is a warning log.");
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "This is an error log.");

	/********************************
	 *		SDL Starts here			*
	 ********************************/
	scc(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window *window = scp(SDL_CreateWindow("Text Editor", 400, 400, 800, 600,
											  SDL_WINDOW_RESIZABLE));

	SDL_Renderer *renderer =
		scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

	SDL_Surface *font_surface =
		surface_from_file("./fonts/charmap-oldschool_white.png");
	SDL_Texture *font_texture =
		scp(SDL_CreateTextureFromSurface(renderer, font_surface));
	SDL_Rect font_rect = {
		.x = 0,
		.y = 0,
		.w = font_surface->w,
		.h = font_surface->h,
	};
	bool quit = false;
	while (!quit) {
		SDL_Event event = {0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					quit = true;
				} break;
			}
			scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
			scc(SDL_RenderClear(renderer));
			scc(SDL_RenderCopy(renderer, font_texture, &font_rect, &font_rect));
			SDL_RenderPresent(renderer);
		}
	}

	// Clean up and quit SDL
	fclose(log_file);
	SDL_FreeSurface(font_surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}