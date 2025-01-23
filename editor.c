#include "./headers/editor.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define LINE_INIT_CAPACITY 1024
#define EDITOR_INIT_CAPACITY 128

// ================ Line ====================
static void line_grow(Line *line, size_t n) {
	size_t new_capacity = line->capacity;

	assert(new_capacity >= line->size);
	while (new_capacity - line->size < n) {
		if (new_capacity == 0) {
			new_capacity = LINE_INIT_CAPACITY;
		} else {
			new_capacity *= 2;
		}
	}
	if (new_capacity != line->capacity) {
		line->chars = realloc(line->chars, new_capacity);
		line->capacity = new_capacity;
	}
}

void line_insert_text_before(Line *line, const char *text, size_t *col) {
	if (*col > line->size) {
		*col = line->size;
	}

	const size_t text_size = strlen(text);

	line_grow(line, text_size);
	memmove(line->chars + *col + text_size, line->chars + *col,
			line->size - *col);
	memcpy(line->chars + *col, text, text_size);
	line->size += text_size;
	*col += text_size;
}
void line_backspace(Line *line, size_t *col) {
	if (*col > line->size) {
		*col = line->size;
	}

	if (*col > 0 && line->size > 0) {
		memmove(line->chars + *col - 1, line->chars + *col, line->size - *col);
		line->size -= 1;
		*col -= 1;
	}
}
void line_delete(Line *line, size_t *col) {
	if (*col > line->size) {
		*col = line->size;
	}

	if (*col < line->size && line->size > 0) {
		memmove(line->chars + *col, line->chars + *col + 1, line->size - *col);
		line->size -= 1;
	}
}

// ================ Editor ====================

static void editor_grow(Editor *editor, size_t reqd_lines) {
	size_t new_capacity = editor->capacity;

	assert(new_capacity >= editor->size);
	while (new_capacity - editor->size < reqd_lines) {
		if (new_capacity == 0) {
			new_capacity = EDITOR_INIT_CAPACITY;
		} else {
			new_capacity *= 2;
		}
	}
	if (new_capacity != editor->capacity) {
		editor->lines =
			realloc(editor->lines, new_capacity * sizeof(editor->lines[0]));
		editor->capacity = new_capacity;
	}
}

void editor_push_new_line(Editor *editor) {
	editor_grow(editor, 1);
	
	editor->size += 1;
}

void editor_insert_text_before_cursor(Editor *editor, const char *text) {
	// assert(false && "todo");

	if (editor->cursor_row >= editor->size) {
		if (editor->size > 0) {
			editor->cursor_row = editor->size - 1;
		} else {
			editor_push_new_line(editor);
		}
	}

	line_insert_text_before(&editor->lines[editor->cursor_row], text,
							&editor->cursor_col);
}
void editor_backspace(Editor *editor) {
	if (editor->cursor_row >= editor->size) {
		if (editor->size > 0) {
			editor->cursor_row = editor->size - 1;
		} else {
			editor_push_new_line(editor);
		}
	}

	line_backspace(&editor->lines[editor->cursor_row], &editor->cursor_col);
}
void editor_delete(Editor *editor) {
	if (editor->cursor_row >= editor->size) {
		if (editor->size > 0) {
			editor->cursor_row = editor->size - 1;
		} else {
			editor_push_new_line(editor);
		}
	}
	line_delete(&editor->lines[editor->cursor_row], &editor->cursor_col);
}