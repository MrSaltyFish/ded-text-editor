#include "./sdl_extra.h"

#include "./stb_image.h"

/*
 * SDL Check Code - Basically panics if the code is negative, as that is the
 * indication of a SDL error. It is a standard thing to do.
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
 */
void *scp(void *ptr) {
	if (ptr == NULL) {
		fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
		exit(1);
	}

	return ptr;
}

void *scp_log(void *ptr, FILE *log) {
	if (ptr == NULL) {
		fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
		fprintf(log, "SDL ERROR: %s\n", SDL_GetError());
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL ERROR: %s\n", SDL_GetError());
		exit(900);
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
		fprintf(stderr, "ERROR: could not load file %s: %s\n", file_path,
				stbi_failure_reason());
		exit(1);
	}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const Uint32 rmask = 0xff000000;
	const Uint32 gmask = 0x00ff0000;
	const Uint32 bmask = 0x0000ff00;
	const Uint32 amask = 0x000000ff;
#else  // little endian, like x86
	const Uint32 rmask = 0x000000ff;
	const Uint32 gmask = 0x0000ff00;
	const Uint32 bmask = 0x00ff0000;
	const Uint32 amask = 0xff000000;
#endif

	const int depth = 32;
	const int pitch = 4 * width;

	return scp(SDL_CreateRGBSurfaceFrom((void *)pixels, width, height, depth,
										pitch, rmask, gmask, bmask, amask));
}

void set_texture_color(SDL_Texture *texture, Uint32 color) {
	scc(SDL_SetTextureColorMod(texture, (color >> (8 * 0)) & 0xff,
							   (color >> (8 * 1)) & 0xff,
							   (color >> (8 * 2)) & 0xff));

	scc(SDL_SetTextureAlphaMod(texture, (color >> (8 * 3)) & 0xff));
}

Vec2f window_size(SDL_Window *window) {
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	return vec2f((float)w, (float)h);
}