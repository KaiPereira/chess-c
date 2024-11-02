/*** dependencies ***/
#include <unistd.h>
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

	printf("\n");
}

int row_num(char x) {
	switch(tolower(x)) {
		case 'a': return 0;
		case 'b': return 1;
		case 'c': return 2;
		case 'd': return 3;
		case 'e': return 4;
		case 'f': return 5;
		case 'g': return 6;
		case 'h': return 7;
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

bool check_pos(const char *input, int *row, int *col) {
	if (input[0] < 'a' || input[0] > 'h' || input[1] < '1' || input[1] > '8') return false;

	*col = input[0] - 'a';
	*row = 8 - (input[1] - '0');

	return true;
}




/*** movegen ***/

/*** legality thanks to https://github.com/JDSherbert for some movegen logic ***/
bool pawn_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	enum Color color = board[x][y].color;
	enum Type type = board[pX][pY].type;

	if (color == white) {
                if (x == 6 && pX == 4 && y == pY && type == empty) return true;
                if (x - 1 == pX && y == pY && type == empty) return true;
                if (x - 1 == pX && (y - 1 == pY || y + 1 == pY) && color == black) return true;
	} else {
                if (x == 1 && pX == 3 && y == pY && type == empty) return true;
                if (x + 1 == pX && y == pY && type == empty) return true;
                if (x + 1 == pX && (y - 1 == pY || y + 1 == pY) && color == white) return true;
	}	

	return false;
}

bool knight_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	if ((x - pX == 2) && (y - pY == 1) || (x - pX == 1) && (x - pY == 2)) return true;
	else return false;
}

bool bishop_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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

bool rook_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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

bool queen_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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

bool king_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	if ((x - pX <= 1) && (y - pY <= 1)) return true;
	else return false;
}

bool make_move(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	enum Type type = board[x][y].type;

	switch(type){
		case pawn: return pawn_rule(board, x, y, pX, pY);
		case knight: return knight_rule(board, x, y, pX, pY);
		case bishop: return bishop_rule(board, x, y, pX, pY);
		case rook: return rook_rule(board, x, y, pX, pY);
		case queen: return queen_rule(board, x, y, pX, pY);
		case king: return king_rule(board, x, y, pX, pY);
		case empty: printf("Cannot move an empty square \n"); return false;
		default: return false;
	}
}


/*** gameloop ***/
void game(struct Piece board[ROW][COL]) {
	int x;
	int y;
	int pX;
	int pY;

	int i;
	
	printf("\e[1;1H\e[2J");

	while (true) { //checkmate
		print_board(board);

		char from[3], to[3];
		int y, x, pY, pX;

		printf("Enter current piece position (e2, b4, etc): ");
		scanf("%2s", from);

		printf("Enter target position (e3, b5, etc): ");
		scanf("%2s", to);

		printf("\e[1;1H\e[2J");

		if (!check_pos(from, &x, &y) || !check_pos(to, &pX, &pY)) {
			printf("Invalid coordinates \n");
			continue;
		}

		if (make_move(board, x, y, pX, pY)) {
			move_piece(board, x, y, pX, pY);
		} else {
			printf("Unable to move that piece there");
		}

		printf("\n");
	}
}


/*** mains ***/
int main() {
	struct Piece board[ROW][COL];
	set_board(STARTING_FEN, board);
	game(board);
}
