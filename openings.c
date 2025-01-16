#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openings.h"

void append_char(char *s, char c) {
    while (*s++);
  
    *(s - 1) = c;
  
    *s = '\0';
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
		int token = 0;
		char eco[sizeof(opening->eco)];
		char name[sizeof(opening->name)];
		char opening[sizeof(opening->moves)];

		for (int i = 0; i < MAX_LINE_LENGTH; i++) {
			char character = line[i];

			if (!character) break;

			if (character == ',') {
 				token++; 
				continue;
			}

			switch (token) {
				case 0:
					append_char(eco, character);
					printf(" %c ", character);
					break;
				case 1:
					append_char(name, character);
					printf(" %c ", character);
					break;
				case 2:
					append_char(name, character);
					printf(" %c ", character);
					break;
			}
		}

		printf("\n");
	}
}
