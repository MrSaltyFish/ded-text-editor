
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

int main(int argc, char **argv) {
	scc(SDL_Init(SDL_INIT_VIDEO));

	SDL_Window *window = scp(SDL_CreateWindow("Text Editor", 400, 400, 800, 600,
											  SDL_WINDOW_RESIZABLE));
	// Create a renderer
	SDL_Renderer *renderer =
		scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

	bool quit = false;
	while (!quit) {
		SDL_Event event = {0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					quit = true;
				} break;
			}

			scc(SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff));
			scc(SDL_RenderClear(renderer));
			SDL_RenderPresent(renderer);
		}
	}

		// Clean up and quit SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}