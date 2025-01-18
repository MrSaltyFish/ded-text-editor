#include <SDL2/SDL.h>
#include <stdio.h>

// Custom log function that writes logs to a file
void my_log_function(void *userdata, int category, SDL_LogPriority priority,
					 const char *message) {
	FILE *log_file = (FILE *)userdata;
	fprintf(log_file, "SDL LOG [%d]: %s\n", priority, message);
}

int main(int argc, char **argv) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n",
				SDL_GetError());
		return 1;
	}

	// Open a log file
	FILE *log_file = fopen("sdl_log.txt", "w");
	if (log_file == NULL) {
		fprintf(stderr, "Could not open log file for writing!\n");
		SDL_Quit();
		return 1;
	}

	// Set the custom log function
	SDL_LogSetOutputFunction(my_log_function, log_file);

	// Log a message
	SDL_Log("This message will go to the log file!");

	// Log messages with different priorities
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "This is an informational log.");
	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "This is a warning log.");
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "This is an error log.");

	// Close the log file
	fclose(log_file);

	// Clean up and quit SDL
	SDL_Quit();
	return 0;
}
