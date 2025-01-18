#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "openings.h"

void append_char(char *s, char c) {
    while (*s) s++;

    *s++ = c;
    *s = '\0';
}

void parse_openings(struct Opening *openings, int max_lines) {
	FILE *file = fopen("openings_test.csv", "r");

	if (!file) {
		perror("Error opening file");
	}

	char line[MAX_LINE_LENGTH];

	int line_count = 0;

	while (fgets(line, sizeof(line), file) && line_count < MAX_LINES) {
		if (line_count == 0) {
			line_count++;
			continue;
		}

		struct Opening *opening = &openings[line_count];
		memset(opening, 0, sizeof(*opening));

		int token = 0;
		char eco[sizeof(opening->eco)] = {0};
		char name[sizeof(opening->name)] = {0};
		char moves_str[sizeof(line)] = {0};

		bool in_title = false;

		for (int i = 0; i < MAX_LINE_LENGTH; i++) {
			char character = line[i];

			if (!character) break;

			if (character == '"' && in_title) in_title = false;
			else if (character == '"') in_title = true;

			if (character == ',' && !in_title) {
 				token++; 
				continue;
			}

			switch (token) {
				case 0:
					append_char(eco, character);
					break;
				case 1:
					append_char(name, character);
					break;
				case 2:
					append_char(moves_str, character);
					break;
			}

		}

		strncpy(opening->eco, eco, sizeof(opening->eco) - 1);
		strncpy(opening->name, name, sizeof(opening->name) - 1);

		char *move = strtok(moves_str, " ");

		while (move && opening->move_count < MAX_MOVES) {
			if (move) {
				strncpy(opening->moves[opening->move_count], move, sizeof(opening->moves[opening->move_count]));

				opening->move_count++;
				move = strtok(NULL, " ");
			} else {
				break;
			}
		}
	}

	fclose(file);
}
