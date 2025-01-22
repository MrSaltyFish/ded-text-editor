#ifndef EDITOR_H_
#define EDITOR_H_
#include <SDL2/SDL.h>
#include <stdlib.h>

typedef struct {
	char *chars;
	size_t capacity;
	size_t size;
} Line;

void line_insert_text_before(Line *line, const char *text, size_t col);
void line_backspace(Line *line, size_t col);
void line_delete(Line *line, size_t col);

typedef struct {
	Line *lines;
	size_t capacity;
	size_t size;
	size_t cursor_row;
	size_t cursor_col;

} Editor;

void editor_insert_text_before_cursor(Editor *editor, const char *text);
void editor_backspace(Editor *editor);
void editor_delete(Editor *editor);

#endif	// EDITOR_H_