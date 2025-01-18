#ifndef STDLIB_H_
#define STDLIB_H_

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Custom log function that writes logs to a file.
 * The function writes the category, priority, and the message to the provided
 * log file.
 * Ensure the log file pointer is valid and opened for writing.
 */
void textLogger(void *userdata, int category, SDL_LogPriority priority,
				const char *message) {
	// Ensure userdata is a valid pointer to a file
	if (userdata == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Log file pointer is NULL!\n");
		return;
	}

	FILE *log_file = (FILE *)userdata;

	// Check if the file is valid (open for writing)
	if (log_file == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to write to log file!\n");
		return;
	}
	// fprintf(log_file, "[Priority %d] [Category %d] SDL-LOG: %s\n", priority,
	// 		category, message);
	// fflush(log_file);  // Make sure to flush each message

	// Define category and priority strings
	char category_str[16];
	char priority_str[16];

	// Map category integers to category names
	switch (category) {
		case SDL_LOG_CATEGORY_APPLICATION:
			strncpy(category_str, "App", sizeof(category_str));
			break;
		case SDL_LOG_CATEGORY_ERROR:
			strncpy(category_str, "Err", sizeof(category_str));
			break;
		case SDL_LOG_CATEGORY_SYSTEM:
			strncpy(category_str, "Sys", sizeof(category_str));
			break;
		case SDL_LOG_CATEGORY_ASSERT:
			strncpy(category_str, "Asr", sizeof(category_str));
			break;
		default:
			strncpy(category_str, "MSC", sizeof(category_str));
			break;
	}

	// Map priority integers to priority names
	switch (priority) {
		case SDL_LOG_PRIORITY_INFO:
			strncpy(priority_str, "Log", sizeof(priority_str));
			break;
		case SDL_LOG_PRIORITY_WARN:
			strncpy(priority_str, "Wrn", sizeof(priority_str));
			break;
		case SDL_LOG_PRIORITY_ERROR:
			strncpy(priority_str, "Err", sizeof(priority_str));
			break;
		case SDL_LOG_PRIORITY_CRITICAL:
			strncpy(priority_str, "Imp", sizeof(priority_str));
			break;
		default:
			strncpy(priority_str, "MSC", sizeof(priority_str));
			break;
	}

	// Write the log message to the file
	fprintf(log_file, "[%s] [%s] SDL-LOG: %s\n", priority_str, category_str,
			message);
}

#endif	// STDLIB_H_
