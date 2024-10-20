/*** dependencies ***/
#include <string.h>
#include <stdio.h>


/*** definition ***/
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


/*** types ***/
enum Type {
	NONE = 1,
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};

enum Color {
	BLACK = 1,
	WHITE
};

struct Piece {
	enum Type type;
	enum Color color;
};


/*** initialisation ***/
static void board_initialisation(char fen[]) {
	struct Piece board[64];

	size_t row, i, column;

	for (i = 0; i < strlen(fen); i++) {
		if (fen[i] == '/') {
			row++;
		}

		printf("%zu \n", row);	
	}	
}       


/*** mains ***/
int main() {
	board_initialisation(STARTING_FEN);

	return 1;
}
