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

void parse_openings(struct Opening *openings) {
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

void play_opening(char moves_played[MAX_MOVES][10]) {
	if (strlen(moves_played[0]) == 0) {
		printf("e2e4 \n");
	}

	int opening = 0;
	int move_to_play = 0;

	struct Opening *openings = malloc(MAX_LINES * sizeof(struct Opening));

	if (!openings) {
		perror("Error allocating memory");
	}

	parse_openings(openings);

	// Look at me comment mom
	// Loop over all openings
	/*for (int i = 0; i < MAX_LINES; i++) {
		// Loop over all the moves in that opening
		for (int w = 0; w < MAX_MOVES; w++) {
			// if the opening moves match the moves played
			// Play the next move in that opening
			
			// If any of the moves played don't match the opening, stop
			if (strcmp(openings[i].moves[w], moves_played[w]) != 0) break;

			// If all the moves match and we've gone through all moves played, play the opening
			if (!moves_played[w + 1][0] != '\0') {
				opening = i;
				move_to_play = w + 1;

				break;
			}
		}
	}*/
	
	// e4 e5 
	// Pe4
	for (int i = 0; i < MAX_LINES; i++) {
		if (strlen(openings[i].name) != 0) {
			for (int w = 0; w < MAX_MOVES; w++) {
				if (strlen(moves_played[w]) == 0) break;

				// if it's a notation like e4 and not Pe4
				if (strlen(openings[i].moves[w]) == 2) {
					char move_played[10];

					// Truncate the move we played to not include P or whatver
					memmove(move_played, moves_played[w] + 1, strlen(moves_played[w]));

					if (strcmp(openings[i].moves[w], move_played) != 0) break;

					if (!moves_played[w + 1][0] != '\0') {
						opening = i;
						move_to_play = w + 1;

						break;
					}
				} else {
					if (strcmp(openings[i].moves[w], moves_played[w]) != 0) break;

					if (!moves_played[w + 1][0] != '\0') {
						opening = i;
						move_to_play = w + 1;

						break;
					}
				}
			}
		}
	}

	free(openings);
}
