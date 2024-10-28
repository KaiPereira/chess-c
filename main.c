/*** dependencies ***/
#include <stdbool.h>
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
	pawn,
	knight,
	bishop,
	rook,
	queen,
	king 
};

struct Piece {
	enum Color color;
	enum Type type;
};


/*** Helper Functions ***/
void set_piece(struct Piece board[ROW][COL], int row, int col, enum Color color, enum Type type) {
	board[row][col].color = color;
	board[row][col].type = type;
}

void move_piece(struct Piece board[ROW][COL], int from_row, int from_col, int to_row, int to_col) {
	board[to_row][to_col] = board[from_row][from_col];

	board[from_row][from_col].type = empty;
	board[from_row][from_col].color = none;
}	

enum Type get_type_enum(char piece) {
	switch(tolower(piece)) {
		case 'p': return pawn;
		case 'n': return knight;
		case 'b': return bishop;
		case 'r': return rook;
		case 'q': return queen;
		case 'k': return king;
		default: return empty;
	}
}

char get_type_char(enum Type type) {
	char piece;

	switch(type) {
		case pawn: piece = 'p'; break;
		case knight: piece = 'n'; break;
		case bishop: piece = 'b'; break;
		case rook: piece = 'r'; break;
		case queen: piece = 'q'; break;
		case king: piece = 'k'; break;
		case empty: piece = '.'; break;
		default: piece = '?';
	}

	return piece;
}

char get_color_char(enum Color color) {
	char p_color;

	switch(color) {
		case white: p_color = 'w'; break;
		case black: p_color = 'b'; break;
		case none: p_color = 'n'; break;
		default: p_color = '?'; break;
	}

	return p_color;
}

void print_board(struct Piece board[COL][ROW]) {
	printf("\n");

	for (size_t r = 0; r < ROW; r++) {
		for (size_t c = 0; c < COL; c++) {
			struct Piece square = board[r][c];

			char color = square.color;
			char type = get_type_char(square.type);

			char f_type = color == white ? toupper(type) : type;

			printf("%c", f_type);
		}
		printf("\n");
	}
}


/** board ***/
void set_board(char fen[], struct Piece board[ROW][COL]) {
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

	for (i = 0; i < BOARD_SIZE; i++) {
		char piece = pieces[i];
		
		int row = i / ROW;
		int col = i % COL;

		if (piece == '.') {
			set_piece(board, row, col, none, empty);
		} else {
			enum Color color = isupper(piece) ? white : black;
			enum Type type = get_type_enum(piece);

			set_piece(board, row, col, color, type);
		}
	}
}


/*** movegen ***/

/*** legality thanks to https://github.com/JDSherbert for some movegen logic ***/
bool pawnRule(struct Piece board[ROW][COL], int y, int x, int pY, int pX) {
	enum Color color = board[x][y].color;
	enum Type type = board[x][y].type;

	if (color == white) {
                if (x == 6 && pX == 4 && y == pY && type == empty) return true;
                if (x - 1 == pX && y == pY && type == empty) return true;
                if (x - 1 == pX && (y - 1 == pY || y + 1 == pY) && color == black) return true;
	} else {
                if (x == 1 && pX == 3 && y == pY && type == empty) return true;
                if (x + 1 == pX && y == pY && type == empty) return true;
                if (x + 1 == pX && (y - 1 == pY || y + 1 == pY) && color == white) return true;
	}	
}

bool knightRule(struct Piece board[ROW][COL], int y, int x, int pY, int pX) {
	if ((x - pX == 2) && (y - pY == 1) || (x - pX == 1) && (x - pY == 2)) return true;
	else return false;
}

bool bishopRule(struct Piece board[ROW][COL], int y, int x, int pY, int pX) {
	if ((y - pY) == (x - pX)) {
		int dx = (x > pX) ? 1 : -1;
		int dy = (y > pY) ? 1 : -1;

		int loop_x = dx + x;
		int loop_y = dy + y;

		while (loop_x != pX) {
			if (board[loop_x][loop_y].type != empty) return false;

			loop_x += dx;
			loop_y += dy;
		}
		
		return true;

	} else return false;
}

bool rookRule(struct Piece board[ROW][COL], int y, int x, int pY, int pX) {
	if ((y == pY) || (x == pX)) {
		int dx = (x == pX) ? 0 : ((pX > x) ? 1 : -1);
                int dy = (y == pY) ? 0 : ((pY > y) ? 1 : -1);

                int loop_x  = x + dx;
                int loop_y  = y + dy;

                while (loop_x != pX || loop_y != pY) {
                    if (board[x][y].type != empty) return false;

                    loop_x += dx;
                    loop_y += dy;
                }

                return true;

	} else return false;
}

bool queenRule(struct Piece board[ROW][COL], int y, int x, int pY, int pX) {
	if ((y == pY) || (x == pX) || (y - pY) == (x - pX)) {
		int dx = (x == pX) ? 0 : ((pX > x) ? 1 : -1);
                int dy = (y == pY) ? 0 : ((pY > y) ? 1 : -1);

                int loop_x = x + dx;
                int loop_y = y + dy;

                while (x != pX || y != pY) {
                    if (board[x][y].type != empty) return false;

                    loop_x += dx;
                    loop_y += dy;
                }

                return true;

	} else return false;
}

bool kingRule(struct Piece board[ROW][COL], int y, int x, int pY, int pX) {
	if ((x - pX <= 1) && (y - pY <= 1)) return true;
	else return false;
}


/*** gameloop ***/
void game(struct Piece board[ROW][COL]) {
	bool valid = pawnRule(board, 1, 0, 2, 0);

	printf("%c", get_type_char(board[1][0].type));

	printf("%d", valid);
}


/*** mains ***/
int main() {
	struct Piece board[ROW][COL];
	set_board(STARTING_FEN, board);
	game(board);

	print_board(board);

	return 1;
}
