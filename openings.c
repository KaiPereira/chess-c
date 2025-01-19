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

void play_opening(/*char moves_played[MAX_MOVES][10]*/) {

	char moves[MAX_MOVES][10] = { "a3", "e5", "h3", "d5" };
	char moves_played[MAX_MOVES][10] = { "a3", "e5" };

	int opening = 0;
	int move_to_play = 0;

	//printf("sizeof moves: %ld", sizeof(moves));


	// Look at me comment mom
	// Loop over all openings
	/*for (int i = 0; i < MAX_LINES; i++) {
	}*/

	// Loop over all the moves in that opening
	for (int w = 0; w < MAX_MOVES; w++) {
		//printf("GOING: %d \n", w);
		// if the opening moves match the moves played
		// Play the next move in that opening
		
		// If any of the moves played don't match the opening, stop
		if (strcmp(moves[w], moves_played[w]) != 0) break;

		// If all the moves match and we've gone through all moves played, play the opening
		if (!moves_played[w + 1][0] != '\0') {
			opening = 0;
			move_to_play = w + 1;

			break;
		}
	}

	printf("move to play: %d", move_to_play);
}
