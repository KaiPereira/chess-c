#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openings.h"


void parse_csv_line(char *line, struct Opening *opening) {
	char *token;
	char *string = line;

	token = strtok_r(string, ",", &string);
	if (token) strncpy(opening->eco, token, sizeof(opening->eco));

	if (string[0] == '"') {
		token = strtok_r(string + 1, "\"", &string);
	} else {
		strtok_r(string, ",", &string);
	}

	if (token) strncpy(opening->name, token, sizeof(opening->name));

	opening->move_count = 0;
	token = strtok_r(string, " ", &string);
	while (token && opening->move_count < MAX_MOVES) {
		strncpy(opening->moves[opening->move_count++], token, 10);
		token = strtok_r(string, " ", &string);
	}
}

void parse_openings(struct Opening *opening) {
	FILE *file = fopen("openings_test.csv", "r");

	if (!file) {
		perror("Error opening file");
	}

	char line[MAX_LINE_LENGTH];


	struct Opening *openings = malloc(MAX_LINES * sizeof(struct Opening));

	if (!openings) {
		perror("failed to allocate memory");
	}

	int line_count = 0;

	while (fgets(line, sizeof(line), file) && line_count < MAX_LINES) {
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {
			if (!line[i]) break;
			printf("%c", line[i]);
		}

		printf("\n");
	}

	fclose(file);


	printf("MAKES IT HERE %d \n", line_count);

	for (int i = 0; i < line_count; i++) {
		printf("ECO: %s, NAME: %s, MOVES:", openings[i].eco, openings[i].name);
		for (int j = 0; j < openings[i].move_count; j++) {
			printf(" %s", openings[i].moves[j]);
		}

		printf("\n");
	}

	// free memory after
	free(openings);
}
