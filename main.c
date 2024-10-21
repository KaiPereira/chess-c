/*** dependencies ***/
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


/*** definition ***/
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define BOARD_SIZE 64
#define ROW 8
#define COL 8

/*** Types ***/
enum Color {
	white, black, none
};

enum Type {
	empty,
	pawn = 'p',
	knight = 'n',
	bishop = 'b',
	rook = 'r',
	queen = 'q',
	king = 'k'
};

struct Piece {
	enum Color color;
	enum Type type;
};


/** Functionality ***/

void format(char fen[]) {
	char pieces[BOARD_SIZE];
	for (int s = 0; s < BOARD_SIZE; s++) pieces[s] = '.';

	size_t fen_len = strlen(fen);
	size_t i = 0;
	int index = 0;

	for (; (i < fen_len) && (fen[i] != ' '); i++) {
		if (fen[i] == '/') continue;
		
		if (isdigit(fen[i])) index += (fen[i] - '0');

		else {
			pieces[index] = fen[i];
			++index;
		}
	}

	struct Piece board[ROW][COL];

	for (i = 0; i < BOARD_SIZE; i++) {
		char square = pieces[i];
		
		int row = i / ROW;
		int col = i / COL;

		struct Piece piece = board[row][col];

		if (square == '.') {
			piece.color = none;
			piece.type = empty;
		} else {
			if (isupper(square)) piece.color = black;
			else piece.color = white;

			piece.color = square;
		}
	}

	int row, columns;
	for (row=0; row<8; row++)
	{
	    for(columns=0; columns<8; columns++)
	    {
		 printf("%d     ",  board[row][columns].type);
	    }
	    printf("\n");
	}
}


/*** mains ***/
int main() {
	format(STARTING_FEN);

	return 1;
}
