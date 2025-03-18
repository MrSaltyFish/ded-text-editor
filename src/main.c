
/*
 *
 *   Ded (Text Editor) - Copied Project from Tsoding Daily -
 * https://www.youtube.com/watch?v=2UY_Am-Q-oI&list=PLpM-Dvs8t0VZVshbPeHPculzFFBdQWIFu
 *	// Haha merge conflict go brr
 */

// Hello from te

//------ Generic Includes -------
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//------- STB_IMAGE ------
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//------- StringView ------
#define SV_IMPLEMENTATION
#include "./sv.h"
//------ Custom Headers -------
#include "editor.h"
#include "file.h"
#include "font.h"
#include "la.h"
#include "sdl_extra.h"
#include "sdllogger.h"
//------ Camera - SDL Stuff -------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 144
#define DELTA_TIME (1.0f / FPS)

#define CAMERA_SPEED 5.0f
//------ OpenGL Stuff -------
#define GLEW_STATIC
#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include "gl_extra.h"
//------ Flags for the app ------
#define OPENGL_RENDERER	 // Comment to run on SDL

/*
 * Input line->chars
 */

// Editor
Editor editor = {0};
Vec2f camera_pos = {0};
Vec2f camera_vel = {0};

Vec2f camera_project_point(SDL_Window *window, Vec2f point) {
	return vec2f_add(vec2f_sub(point, camera_pos),
					 vec2f_mul(window_size(window), vec2fs(0.5f)));
}

/*
 * This is code to just display the cursor
 */
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

	scc(SDL_SetRenderDrawColor(renderer, UNHEX(0xFFFFFFFF)));
	scc(SDL_RenderFillRect(renderer, &rect));

	const char *c = render_char_under_cursor(&editor);
	if (c) {
		set_texture_color(font->spritesheet, 0xFF000000);
		render_char(renderer, font, *c, pos, FONT_SCALE);
	}
}

void usage(FILE *stream) { fprintf(stream, "Usage: te [FILE-PATH]\n"); }

// TODO: Save File
// TODO: ncurses renderer
// TODO: Jump forward/backward by a word
// TODO: Delete a word
// TODO: Blinking cursor
// TODO: Delete a line
// TODO: Split the line on enter

#ifdef OPENGL_RENDERER

void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
					 GLsizei length, const GLchar *message,
					 const void *userParam) {
	(void)source;
	(void)id;
	(void)length;
	(void)userParam;

	fprintf(stderr,
			"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

void MessageCallback_Log(GLenum source, GLenum type, GLuint id, GLenum severity,
						 GLsizei length, const GLchar *message,
						 const void *userParam, FILE *log) {
	MessageCallback(source, type, id, severity, length, message, userParam);

	fprintf(log, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	scc(SDL_Init(SDL_INIT_VIDEO));

	FILE *log = NULL;
	log = fopen("build/opengl_log.txt", "w");
	if (log == NULL) {
		printf("Cannot open log file!");
		exit(404);
	} else {
		fprintf(log, "Initializing...\n");
	}
	SDL_Window *window = scp(
		SDL_CreateWindow("Text Editor", 200, 400, SCREEN_WIDTH, SCREEN_HEIGHT,
						 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
	fprintf(log, "SDL_Window Created...\n");
	fprintf(log, "Setting SDL_GL Attributes\n");

	// OpenGL Version Construct
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
							SDL_GL_CONTEXT_PROFILE_CORE);
		int major;
		int minor;

		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		fprintf(log, "GL Version: %d.%d\n", major, minor);
	}
	fprintf(log, "SDL_GL Attributes set: OpenGL Version 3.3\n");
	SDL_GLContext glContext = scp(SDL_GL_CreateContext(window));
	fprintf(log, "Created Context\n");

	if (GLEW_OK != glewInit()) {
		fprintf(log, "Could not initialize GLEW!\n");
		exit(110);
	}
	fprintf(log, "Initialized GLEW!\n");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (GLEW_ARB_debug_output) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
		fprintf(log, "DEBUG! GLEW_ARB_debug_output is available!\n");

	} else {
		fprintf(log, "Warning! GLEW_ARB_debug_output is not available!\n");
	}

	GLuint vert_shader = 0;
	if (!compile_shader_file("./shaders/font.vert", GL_VERTEX_SHADER,
							 &vert_shader, log)) {
		exit(404);
	}

	GLuint frag_shader = 0;
	if (!compile_shader_file("./shaders/font.frag", GL_FRAGMENT_SHADER,
							 &frag_shader, log)) {
		exit(405);
	}

	GLuint program = 0;

	if (!link_program(vert_shader, frag_shader, &program)) {
		fprintf(log, "Cannot Link Program\n");
		exit(406);
	}
	fprintf(log, "Linked Program\n");

	glUseProgram(program);

	bool quit = false;

	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_g: {
							quit = true;
						}
					}
				}
			}
		}
	}

	fclose(log);

	return 0;
}

#else
/********************************
 *		SDL MAIN Starts here	*
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
#endif