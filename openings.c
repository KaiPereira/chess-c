#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "openings.h"

void append_char(char *s, char c) {
    while (*s) s++;

    *s++ = c;
    *s = '\0';
}

void trim(char *str) {
    while (isspace(*str)) str++;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
}

void parse_openings(struct Opening *openings) {
	FILE *file = fopen("openings.csv", "r");

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

		struct Opening *opening = &openings[line_count - 1];
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

		line_count++;
	}

	fclose(file);
}

void play_opening(char moves_played[MAX_MOVES][10], char opening_move[10]) {
	int opening = 0;
	int move = 0;

	struct Opening *openings = malloc(MAX_LINES * sizeof(struct Opening));

	if (!openings) {
		perror("Error allocating memory");
	}

	parse_openings(openings);

	for (int i = 0; i < MAX_LINES && strlen(openings[i].name); i++) {
		printf("%d \n", i);
		for (int j = 0; j < MAX_MOVES; j++) {
			char trunc_move[10];
			
			if (strlen(openings[i].moves[j]) == 2) {
				memmove(trunc_move, moves_played[j] + 1, strlen(moves_played[j]));

				if (strcmp(openings[i].moves[j], trunc_move) != 0) break;
			} else {
				if (strcmp(openings[i].moves[j], moves_played[j]) != 0) break;
			}

			if (strlen(moves_played[j + 1]) == 0) {
				opening = i;
				move = j + 1;
			}
		}
	}


	//printf("Name: %s, move: %s \n", openings[opening].name, openings[opening].moves[move]);
	
	trim(openings[opening].moves[move]);

	strcpy(opening_move, openings[opening].moves[move]);

	free(openings);
}
