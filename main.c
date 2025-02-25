/*** dependencies ***/
#include <limits.h>
#include <time.h>
#include <term.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/*** local dependencies ***/
#include "positions.h"
#include "openings.h"


/***
   y y y y y y y y
x  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ 
x  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙ 
x  . . . . . . . . 
x  . . . . . . . . 
x  . . . . . . . . 
x  . . . . . . . . 
x  ♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ 
x  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ 
 ***/

// Y IS LETTERS
// X IS NUMBERS


/*** definition ***/
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//#define STARTING_FEN "rnbqkbnr/pppppppp/8/1N6/3n4/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
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

struct Move {
	int x, y;
	int pX, pY;
};


/*** Global variables ***/
bool rook1_w_moved = false;
bool rook2_w_moved = false;
bool castle_w = true;

bool rook1_b_moved = false;
bool rook2_b_moved = false;
bool castle_b = true;

enum Color computer_color = white;
enum Color color_to_move = white;

// move history for opening and other shenanigans
char moves_played[MAX_MOVES][10];
int move_history_count = 0;

bool is_opening = true;


/*** General Helper Functions ***/
void clear_scr() {
	//printf("\e[1;1H\e[2J");
}

/*** Helper Functions ***/
void set_piece(struct Piece board[ROW][COL], int row, int col, enum Color color, enum Type type) {
	board[row][col].color = color;
	board[row][col].type = type;
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
	switch(type) {
		case pawn: return 'p';
		case knight: return 'n';
		case bishop: return 'b';
		case rook: return 'r';
		case queen: return 'q';
		case king: return 'k';
		case empty: return '.';
	}
}

wchar_t get_piece_unicode(struct Piece piece) {
	enum Type type = piece.type;
	enum Color color = piece.color;

	// idk why this colored is reversed tbh, but the color is right but its being weird :/
	if (color == black) {
		switch(type) {
			case pawn: return L'\u2659';
			case knight: return L'\u2658';
			case bishop: return L'\u2657';
			case rook: return L'\u2656';
			case queen: return L'\u2655';
			case king: return L'\u2654';
		}
	} else if (color == white) {
		switch(type) {
			case pawn: return L'\u265F';
			case knight: return L'\u265E';
			case bishop: return L'\u265D';
			case rook: return L'\u265C';
			case queen: return L'\u265B';
			case king: return L'\u265A';
		}
	} else return '.';
}

char get_color_char(enum Color color) {
	switch(color) {
		case white: return 'w';
		case black: return 'b';
		case none: return 'n';
		default: return '?';
	}
}

void print_board(struct Piece board[COL][ROW]) {
	printf("\n");

	bool isWhite = (color_to_move == white);

	// an extra row for letters
	// goofy ah for loop, how do u make this stuff readable and simple :/
	for (int r = isWhite ? 0 : (ROW - 1); isWhite ? (r < ROW + 1) : (r >= -1); isWhite ? (r++) : (r--)) {
		if (isWhite ? (r == ROW) : (r == -1)) printf("\n   a b c d e f g h");
		
		else {
			// bruh this was some straight up prodigy level stuff
			printf("%d  ", abs(r - 8));

			for (size_t c = 0; c < COL; c++) {
				struct Piece square = board[r][c];

				wchar_t symbol = get_piece_unicode(square);

				printf("%lc ", symbol);
			}
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



// Turns number row/col into coordinates
bool check_pos(const char *input, int *row, int *col) {
	if (input[0] < 'a' || input[0] > 'h' || input[1] < '1' || input[1] > '8') return false;

	*col = input[0] - 'a';
	*row = 8 - (input[1] - '0');

	return true;
}

void find_piece(struct Piece board[ROW][COL], enum Color color, int *x, int *y) {
	int r, c;

	for (r = 0; r < ROW; r++) {
		for (c = 0; c < COL; c++) {
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

void change_turn() {
	color_to_move = reverse_color(color_to_move);
}

struct Move create_move(int x, int y, int pX, int pY) {
	struct Move move;
	move.x = x;
	move.y = y;
	move.pX = pX;
	move.pY = pY;

	return move;
}

void print_move(struct Move move) {
	char row1 = 'a' + move.y;
	char row2 = 'a' + move.pY;

	printf("Move: %c%d to %c%d \n", row1, 8 - move.x, row2, 8 - move.pX);
}

void move_piece(struct Piece board[ROW][COL], struct Move move) {
	board[move.pX][move.pY] = board[move.x][move.y];

	board[move.x][move.y].type = empty;
	board[move.x][move.y].color = none;
}

void convert_to_algebraic(struct Piece board[ROW][COL], struct Move move, char formatted_move[10]) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	struct Piece piece = board[x][y];

	enum Color color = piece.color;

	char type_char = get_type_char(piece.type);
	char to_square = 'a' + move.pY;

	type_char = color == white ? toupper(type_char) : type_char;

	formatted_move[0] = type_char;
	formatted_move[1] = to_square;
	formatted_move[2] = '0' + pX;
	formatted_move[3] = '\0';
}

/*** movegen ***/
/*** pseudolegality thanks to https://github.com/JDSherbert for some movegen logic ***/
// en passant is stupid :/
bool pawn_rule(struct Piece board[ROW][COL], struct Move move) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	if (x < 0 || x >= ROW || y < 0 || y >= COL || pX < 0 || pX >= ROW || pY < 0 || pY >= COL) {
		return false;
	}

	if (board[x][y].type != pawn) return false;

	enum Color color = board[x][y].color;
	enum Type type = board[pX][pY].type;

	if (color == white) {
		if (x == 6 && pX == 4 && y == pY && type == empty && board[5][y].type == empty) return true;
		if (x - 1 == pX && y == pY && type == empty) return true;
		if (x - 1 == pX && (y - 1 == pY || y + 1 == pY) && type != empty && board[pX][pY].color == black) return true;
	} else if (color == black) {
		if (x == 1 && pX == 3 && y == pY && type == empty && board[2][y].type == empty) return true;
		if (x + 1 == pX && y == pY && type == empty) return true;
		if (x + 1 == pX && (y - 1 == pY || y + 1 == pY) && type != empty && board[pX][pY].color == white) return true;
	}

	return false;
}


bool knight_rule(struct Piece board[ROW][COL], struct Move move) {
    int x = move.x;
    int y = move.y;
    int pX = move.pX;
    int pY = move.pY;

    if (((abs(x - pX) == 2) && (abs(y - pY) == 1)) || 
        ((abs(x - pX) == 1) && (abs(y - pY) == 2))) {
        return true;
    } else {
        return false;
    }
}

bool bishop_rule(struct Piece board[ROW][COL], struct Move move) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	if (abs(y - pY) != abs(x - pX)) return false;

	int dx = (pX > x) ? 1 : -1;
	int dy = (pY > y) ? 1 : -1;

	int loop_x = x + dx;
	int loop_y = y + dy;

	while (loop_x != pX && loop_y != pY) {
		if (loop_x < 0 || loop_x >= ROW || loop_y < 0 || loop_y >= COL) return false;

		if (board[loop_x][loop_y].type != empty) return false;

		loop_x += dx;
		loop_y += dy;
	}

	return true;
}

bool rook_rule(struct Piece board[ROW][COL], struct Move move) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	if (y == pY || x == pX) {
		int dx = (x == pX) ? 0 : ((pX > x) ? 1 : -1);
                int dy = (y == pY) ? 0 : ((pY > y) ? 1 : -1);

                int loop_x  = x + dx;
                int loop_y  = y + dy;

                while (loop_x != pX || loop_y != pY) {
                    if (board[loop_x][loop_y].type != empty) return false;

                    loop_x += dx;
                    loop_y += dy;
                }

                return true;

	} else return false;
}

bool queen_rule(struct Piece board[ROW][COL], struct Move move) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	if (y == pY || x == pX) {
		int dx = (x == pX) ? 0 : ((pX > x) ? 1 : -1);
                int dy = (y == pY) ? 0 : ((pY > y) ? 1 : -1);

                int loop_x  = x + dx;
                int loop_y  = y + dy;

                while (loop_x != pX || loop_y != pY) {
                    if (board[loop_x][loop_y].type != empty) return false;

                    loop_x += dx;
                    loop_y += dy;
                }

                return true;
	}

	if (abs(y - pY) != abs(x - pX)) return false;

	int dx = (pX > x) ? 1 : -1;
	int dy = (pY > y) ? 1 : -1;

	int loop_x = x + dx;
	int loop_y = y + dy;

	while (loop_x != pX && loop_y != pY) {
		if (loop_x < 0 || loop_x >= ROW || loop_y < 0 || loop_y >= COL) return false;

		if (board[loop_x][loop_y].type != empty) return false;

		loop_x += dx;
		loop_y += dy;
	}

	return true;
}

bool king_rule(struct Piece board[ROW][COL], struct Move move) { 
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	if (abs(x - pX) <= 1 && abs(y - pY) <= 1) return true;

	else return false;
}


/*** chess/pins/attacks ***/
struct Move convert_from_algebraic(struct Piece board[ROW][COL], char move[10]) {
	int move_len = strlen(move);

	struct Move opening_move = create_move(0, 0, 0, 0);

	if (move_len == 3) {
		int dest_col = move[1] - 'a';
		int dest_row = 8 - (move[2] - '0');
		enum Type type = get_type_enum(tolower(move[0]));

		for (int x = 0; x < ROW; x++) {
			for (int y = 0; y < COL; y++) {
				if (board[x][y].color != color_to_move) break;
				struct Move movement = create_move(x, y, dest_row, dest_col);

				bool can_move;

				switch(type) {
					case pawn:
						can_move = pawn_rule(board, movement);
						break;
					case knight:
						can_move = knight_rule(board, movement);
						break;
					case bishop:
						can_move = bishop_rule(board, movement);
						break;
					case rook:
						can_move = rook_rule(board, movement);
						break;
					case queen:
						can_move = queen_rule(board, movement);
						break;
					case king:
						can_move = king_rule(board, movement);
						break;
				}

				if (can_move) return movement;
			}
		}
	} else if (move_len == 2) {
		int dest_col = move[0] - 'a';
		int dest_row = 8 - (move[1] - '0');
		
		for (int x = 0; x < ROW; x++) {
			for (int y = 0; y < COL; y++) {
				if (board[x][y].color != color_to_move) continue;

				struct Move movement = create_move(x, y, dest_row, dest_col);

				//printf("%d, %d, %d, %d \n", x, y, dest_row, dest_col);

				enum Type type = board[x][y].type;

				bool can_move;

				switch(type) {
					case pawn:
						can_move = pawn_rule(board, movement);
						break;
					case knight:
						can_move = knight_rule(board, movement);
						break;
					case bishop:
						can_move = bishop_rule(board, movement);
						break;
					case rook:
						can_move = rook_rule(board, movement);
						break;
					case queen:
						can_move = queen_rule(board, movement);
						break;
					case king:
						can_move = king_rule(board, movement);
						break;
				}

				if (can_move) return movement;			
			}
		}
	}

	return opening_move;
}

void add_move_history(struct Piece board[ROW][COL], struct Move move) {
	char formatted_move[10];

	convert_to_algebraic(board, move, formatted_move);

	strncpy(moves_played[move_history_count], formatted_move, sizeof(moves_played[0]));
}

bool king_attacked(struct Piece board[ROW][COL]) {
	int x, y;

	find_piece(board, color_to_move, &x, &y);

	for (int r = 0; r < ROW; r++) {
		for (int c = 0; c < COL; c++) {
			enum Type type = board[r][c].type;
			char color = board[r][c].color;

			bool is_attacked = false;

			if (type == empty) continue;
			if (color == color_to_move) continue;

			struct Move move = create_move(r, c, x, y);

			switch(type) {
				case pawn: is_attacked = pawn_rule(board, move); break;
				case knight: is_attacked = knight_rule(board, move); break;
				case bishop: is_attacked = bishop_rule(board, move); break;
				case rook: is_attacked = rook_rule(board, move); break;
				case queen: is_attacked = queen_rule(board, move); break;
			}
	
			if (is_attacked) return true;
		}
	}

	return false;
}

bool square_attacked(struct Piece board[ROW][COL], int pX, int pY) {
	for (int x = 0; x < ROW; x++) {
		for (int y = 0; y < COL; y++) {
			enum Color color = board[x][y].color;
			enum Type type = board[x][y].type;

			if (color == none || color == color_to_move) continue;

			struct Move move = create_move(x, y, pX, pY);

			bool is_attacked = false;

			switch(type) {
                                 case pawn: is_attacked = pawn_rule(board, move); break;
                                 case knight: is_attacked = knight_rule(board, move); break;
                                 case bishop: is_attacked = bishop_rule(board, move); break;
                                 case rook: is_attacked = rook_rule(board, move); break;
                                 case queen: is_attacked = queen_rule(board, move); break;
                         }
         
                         if (is_attacked) return true;
		}
	}

	return false;
}

bool castle_rights(
		struct Piece board[ROW][COL], 
		struct Move move
) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	enum Color color = board[x][y].color;
	enum Type type = board[x][y].type;

	bool valid = false;

	if (type == king && x == pX && y == 4) {
		if (color == white) {
			if (castle_w) {
				if (pY == 2 && !rook2_w_moved) {
					for (int x = 4; x >= 2; x--) {
						if (board[7][x - 1].type == empty && !square_attacked(board, 7, x)) valid = true;
					}
				} else if (pY == 6 && !rook1_w_moved) {
					for (int x = 4; x <= 6; x++) {
						if (board[7][x + 1].type == empty && !square_attacked(board, 7, x)) valid = true;
					}
				}
			}
		} else if (color == black) {
			if (castle_b) {
				if (pY == 2 && !rook2_b_moved) {
					for (int x = 4; x >= 2; x--) {
						if (board[0][x - 1].type == empty && !square_attacked(board, 0, x)) valid = true;
					}
				} else if (pY == 6 && !rook1_b_moved) {
					for (int x = 4; x <= 6; x++) {
						if (board[0][x + 1].type == empty && !square_attacked(board, 0, x)) valid = true;
					}
				}
			}
		}
	}

	return valid;
}

void castle(struct Piece board[ROW][COL], struct Move move) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	move_piece(board, move);

	if (move.pY == 6) { // right side castle
		move_piece(board, create_move(x, 7, pX, 5));
	} else if (move.pY == 2) { // left
		move_piece(board, create_move(x, 0, pX, 3));
	}
}


// This implements castling too
void make_move(struct Piece board[ROW][COL], struct Move move) {
	if (castle_rights(board, move)) {
		castle(board, move);
	} else {
		move_piece(board, move);
	}
}



/*** final move gen ***/
bool check_move(struct Piece board[ROW][COL], struct Move move) {
	enum Type type = board[move.x][move.y].type;

	if (castle_rights(board, move)) return true;

	switch(type) {
		case pawn: return pawn_rule(board, move);
		case knight: return knight_rule(board, move);
		case bishop: return bishop_rule(board, move);
		case rook: return rook_rule(board, move);
		case queen: return queen_rule(board, move);
		case king: return king_rule(board, move);
		case empty: return false;
		default: return false;
	}
}

bool board_status(struct Piece board[ROW][COL], struct Move move) {
	int x = move.x;
	int y = move.y;
	int pX = move.pX;
	int pY = move.pY;

	// Save the state of the affected squares
	struct Piece original_piece = board[pX][pY];
	struct Piece moved_piece = board[x][y];

	board[pX][pY] = moved_piece;
	board[x][y].type = empty;

	bool attacked = king_attacked(board);

	board[x][y] = moved_piece;
	board[pX][pY] = original_piece;

	return attacked;
}

int legal_moves(struct Piece board[ROW][COL], struct Move moves[256], enum Color color) {
	int moves_count = 0; 

	for (int x = 0; x < ROW; x++) {
		for (int y = 0; y < COL; y++) {
			if (board[x][y].color != color) continue;

			// Generate pseudo-legal moves for this piece
			for (int pX = 0; pX < ROW; pX++) {
				for (int pY = 0; pY < COL; pY++) {
					struct Move move = create_move(x, y, pX, pY);

					if (castle_rights(board, move)) {
						moves[moves_count] = move;

						moves_count++;

						continue;
					}

					if (!check_move(board, move)) continue;

					if (board[x][y].color != color) continue;

					if (board[x][y].color == board[pX][pY].color) continue;

					if (board_status(board, move)) continue;

					moves[moves_count] = move;

					moves_count++;
				}
			}
		}
	}

	return moves_count;
}

/*** perft ***/
unsigned long perft(struct Piece board[ROW][COL], int depth, enum Color cur_color) {
	if (!depth) {
		return 1;    
	}

	unsigned long nodes = 0;

	struct Move moves[256];
	legal_moves(board, moves, cur_color);

	for (int i = 0; i < sizeof(moves) / sizeof(moves[0]); i++) {
		struct Move move = moves[i];

		// Save the pieces involved in the move
		struct Piece moving_piece = board[move.x][move.y];
		struct Piece captured_piece = board[move.pX][move.pY];

		char node_type = get_type_char(moving_piece.type);

		// Make the move
		make_move(board, moves[i]);

		// Check legality (doesn't leave own king in check)
		nodes += perft(board, depth - 1, reverse_color(color_to_move));

		// Undo the move
		board[move.x][move.y] = moving_piece;
		board[move.pX][move.pY] = captured_piece;
	}

	return nodes;
}


void test_perft(struct Piece board[ROW][COL], int depth) {
	clock_t start_time = clock();

	unsigned long nodes = perft(board, depth, color_to_move);

	clock_t end_time = clock();

	printf("Perft at depth %d: %lu nodes \n", depth, nodes);

	double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	printf("Time taken: %.3f seconds \n", time_spent);
}

/*** evaluation ***/
int evaluate(struct Piece board[ROW][COL]) {
	int pawn_value = 100, knight_value = 300, bishop_value = 350;
	int rook_value = 500, queen_value = 800, king_value = 100000;

	int eval = 0;

	for (int x = 0; x < ROW; x++) {
		for (int y = 0; y < COL; y++) {
			if (board[x][y].type == empty) continue;

			enum Type type = board[x][y].type;
			enum Color color = board[x][y].color;

			int sign = (color == white) ? 1 : -1;
			int position_value = 0, type_value = 0;

			switch(type) {
				case pawn: 
					position_value = (color == white) ? pawn_table[x][y] : pawn_table[7 - x][y];
					type_value = pawn_value;
					break;
				case knight: 
					position_value = (color == white) ? knight_table[x][y] : knight_table[7 - x][y];
					type_value = knight_value;
					break;
				case bishop: 
					position_value = (color == white) ? bishop_table[x][y] : bishop_table[7 - x][y];
					type_value = bishop_value;
					break;
				case rook: 
					position_value = (color == white) ? rook_table[x][y] : rook_table[7 - x][y];
					type_value = rook_value;
					break;
				case queen: 
					position_value = (color == white) ? queen_table[x][y] : queen_table[7 - x][y];
					type_value = queen_value;
					break;
				case king: 
					position_value = (color == white) ? king_table[x][y] : king_table[7 - x][y];
					type_value = king_value;
					break;
				default:
					fprintf(stderr, "Invalid piece type: %d\n", type);
					break;
			}

			eval += sign * (position_value + type_value);
		}
	}

	return eval;
}

int minimax(struct Piece board[ROW][COL], int depth, int alpha, int beta, enum Color color, struct Move *best_move) {
	if (depth == 0) {
		return evaluate(board);
	}

	struct Move moves[256];
	int moves_count = legal_moves(board, moves, color);

	int best_move_index;


	if (color == white) {
		int max_value = INT_MIN;

		for (int i = 0; i < moves_count; i++) {
			struct Piece temp_board[ROW][COL];

			memcpy(temp_board, board, sizeof(struct Piece) * ROW * COL);

			make_move(temp_board, moves[i]);

			int value = minimax(temp_board, depth - 1, alpha, beta, reverse_color(color), NULL);

			if (value > max_value) {
				max_value = value;

				if (best_move != NULL) {
					*best_move = moves[i]; // Update the best move
				}
			}

			alpha = (value > alpha) ? value : alpha;

			if (beta <= alpha) {
				break;
			}
		}

		return max_value;
	} else {
		int min_value = INT_MAX;

		for (int i = 0; i < moves_count; i++) {
			struct Piece temp_board[ROW][COL];

			memcpy(temp_board, board, sizeof(struct Piece) * ROW * COL);

			make_move(temp_board, moves[i]);

			int value = minimax(temp_board, depth - 1, alpha, beta, reverse_color(color), NULL);

			if (min_value > value) {
				min_value = value;

				if (best_move != NULL) {
					*best_move = moves[i]; // Update the best move
				}
			}

			beta = (value < beta) ? value : beta;

			if (beta <= alpha) {
				break;
			}
		}

		return min_value;
	}
}

/*** gameloop ***/
void game(struct Piece board[ROW][COL]) { 
	int x;
	int y;
	int pX;
	int pY;

	int i;
	
	clear_scr();

	while (true) {

		char opening_move[10];
		play_opening(moves_played, opening_move);

		if (strlen(opening_move) != 0) {
			printf("opening: %s \n", opening_move);
			struct Move converted_move = convert_from_algebraic(board, opening_move);

			printf("\n");
			print_move(converted_move);
		}

		// Computers turn
		/*if (color_to_move == computer_color) {
			if (is_opening) {
				char opening_move[10];
				play_opening(moves_played, opening_move);

				if (strlen(opening_move) != 0) {
					struct Move converted_move = convert_from_algebraic(board, opening_move);

					move_piece(board, converted_move);

					change_turn();

					continue;
				}
			}
			
			struct Move best_move;

			int best_value = minimax(board, 3, INT_MIN, INT_MAX, color_to_move, &best_move);

			printf("BEST VALUE: %d \n", best_value);
			print_move(best_move);

			move_piece(board, best_move);

			print_board(board);

			change_turn();

			continue;
		}*/

		print_board(board);


		char from[3], to[3];

		printf("Enter current piece position (e2, b4, etc): ");
		scanf("%2s", from);

		// check the coordinate
		if (!check_pos(from, &x, &y)) {
			printf("Invalid coordinates \n");
			clear_scr();
			continue;
		}

		printf("Enter target position (e3, b5, etc): ");
		scanf("%2s", to);

		// check coordinates for 2
		if (!check_pos(to, &pX, &pY)) {
			printf("Invalid coordinates \n");
			continue;
		}

		clear_scr();


		// Create a move and check it
		struct Move move = create_move(x, y, pX, pY);

		enum Color color = board[x][y].color;
		enum Type type = board[x][y].type;

		if (color != color_to_move) {
			printf("Wrong color to move \n");
			continue;
		}

		if (color == board[pX][pY].color) {
			printf("Can't move your piece onto your own color");
			continue;
		}

		if (check_move(board, move)) {
			if (board_status(board, move)) {
				printf("You're in check or moving into it. \n");
				continue;
			}

			//add_move_history(board, move);

			make_move(board, move);

			// Change castling rights
			if (color_to_move == white) {
				if (type == rook && y == 7) rook1_w_moved = true;
				else if (type == rook && y == 0) rook2_w_moved = true;
				else if (type == king) castle_w = false;
			} else {
				if (type == rook && y == 7) rook1_b_moved = true;
				else if (type == rook && y == 0) rook2_b_moved = true;
				else if (type == king) castle_b = false;
			}


			change_turn();
		} else {
			printf("Unable to move that piece there");
		}

		printf("\n");
	}
	
}

/*** mains ***/
int main() {
	setlocale(LC_ALL, "");
	struct Piece board[ROW][COL];
	set_board(STARTING_FEN, board);
	game(board);

	return 0;
}
