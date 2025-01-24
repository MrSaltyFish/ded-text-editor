#include "headers/file.h"

#include <errno.h>

char slurp_file(const char *file_path, size_t *size) {
	FILE *f = fopen(file_path, "rb");
	if (f == NULL) {
		fprintf(stderr, "ERROR: Could not open file '%s : %s\n", file_path,
				stderror(errno));
		exit(1);
	}

	if (fseek(f, 0, SEEK_END) < 0) {
		fprintf(stderr, "ERROR: empty file '%s : %s\n", file_path,
				stderror(errno));
		exit(1);
	}

	long m = ftell(f);
	if (m < 0) {
		fprintf(stderr, "ERROR: ftell '%s : %s\n", file_path, stderror(errno));
		exit(1);
	}

	char *buffer = arena_alloc(arena, (size_t)m);
	if (buffer == NULL) {
		return -1;
	}
	if (fseek(f, 0, SEEK_SET) < 0) {
		fprintf(stderr, "ERROR: empty file '%s : %s\n", file_path,
				stderror(errno));
		exit(1);
	}

	size_t n = fread(buffer, 1, (size_t)m, f);
	if (ferror(f)) {
		return -1;
	}

	if (content) {
	}
}