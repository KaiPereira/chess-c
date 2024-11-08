/*** dependencies ***/
#include <stdlib.h>
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


/*** General Helper Functions ***/
void clear_scr() {
	printf("\e[1;1H\e[2J");
}

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

void find_piece(struct Piece board[ROW][COL], enum Color color, int *x, int *y) {
	int r, c = 0;

	for (; r < ROW; r++) {
		for (; c < COL; c++) {
			if (board[r][c].type == king && board[r][c].color == color) {
				*x = r;
				*y = c;
			}
		}
	}
}

enum Color reverse_color(enum Color color) {
	if (color == white) {
		return black;
	} else if (color == black) {
		return white;
	}
}


/*** movegen ***/

/*** pseudolegality thanks to https://github.com/JDSherbert for some movegen logic ***/
bool pawn_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
   	if (x < 0 || x >= ROW || y < 0 || y >= COL || pX < 0 || pX >= ROW || pY < 0 || pY >= COL) {
        	return false;
   	}

   	enum Color color = board[x][y].color;
	enum Type type = board[pX][pY].type;

   	if (color == white) {
        	if (x == 6 && pX == 4 && y == pY && type == empty) return true;
        	if (x - 1 == pX && y == pY && type == empty) return true;
        	if (x - 1 == pX && (y - 1 == pY || y + 1 == pY) && board[pX][pY].color == black) return true;
    	} else if (color == black) {
        	if (x == 1 && pX == 3 && y == pY && type == empty) return true;
        	if (x + 1 == pX && y == pY && type == empty) return true;
        	if (x + 1 == pX && (y - 1 == pY || y + 1 == pY) && board[pX][pY].color == white) return true;
    	}

	return false;
}


bool knight_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	if ((abs(x - pX) == 2) && (abs(y - pY) == 1) || (abs(x - pX) == 1) && (abs(x - pY) == 2)) return true;

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


/*** chess/pins ***/
bool king_attacked(struct Piece board[ROW][COL], enum Color king_color) {
	
	int x, y;

	find_piece(board, king_color, &x, &y);

	for (int r = 0; r < ROW; r++) {
		for (int c = 0; c < COL; c++) {
			enum Type type = board[r][c].type;
			char color = board[r][c].color;

			bool isAttacked = false;

			if (type == empty) continue;
			if (color == king_color) continue;

			switch(type) {
				case pawn: isAttacked = pawn_rule(board, r, c, x, y);
				case knight: isAttacked = knight_rule(board, r, c, x, y);
				case bishop: isAttacked = bishop_rule(board, r, c, x, y);
				case rook: isAttacked = rook_rule(board, r, c, x, y);
				case queen: isAttacked = queen_rule(board, r, c, x, y);
			}

			if (isAttacked) return true;
		}
	}

	return false;
}


/*** final move gen ***/
bool check_move(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	enum Type type = board[x][y].type;

	switch(type) {
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

	enum Color color_to_move = white;

	int i;
	
	clear_scr();

	while (true) {
		print_board(board);

		char from[3], to[3];
		int y, x, pY, pX;

		printf("Enter current piece position (e2, b4, etc): ");
		scanf("%2s", from);

		// Make the move and then check if that leaves the king in check
		if (!check_pos(from, &x, &y)) {
			printf("Invalid coordinates \n");
			clear_scr();
			continue;
		}

		printf("Enter target position (e3, b5, etc): ");
		scanf("%2s", to);

		// Make the move and then check if that leaves the king in check
		if (!check_pos(to, &pX, &pY)) {
			printf("Invalid coordinates \n");
			continue;
		}

		clear_scr();


		if (board[x][y].color != color_to_move) {
			printf("Wrong color to move \n");
			continue;
		}

		printf("%c, x%d, y%d, pX%d, pY%d \n", get_type_char(board[x][y].type), x, y, pX, pY);

		if (check_move(board, x, y, pX, pY)) {
			struct Piece temp_board[ROW][COL];

			memcpy(temp_board, board, sizeof(struct Piece) * ROW * COL);

			move_piece(board, x, y, pX, pY);
			
			if (king_attacked(board, color_to_move)) {
				printf("Your king is in check \n");
				memcpy(board, temp_board, sizeof(struct Piece) * ROW * COL);
			}

			color_to_move = reverse_color(color_to_move);
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

	return 0;
}
