#ifndef OPENINGS_H
#define OPENINGS_H


#define MAX_LINES 3000
#define MAX_MOVES 256
#define MAX_LINE_LENGTH 1024

struct Opening {
	char eco[10];
	char name[256];
	char moves[MAX_MOVES][10];
	int move_count;
};

void parse_openings();

void play_opening(char moves_played[MAX_MOVES][10], char opening_move[10]);


#endif
