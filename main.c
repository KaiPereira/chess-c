/*** dependencies ***/
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


/*** definition ***/
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w kq - 0 1"
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
	//printf("\e[1;1H\e[2J");
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

void print_board(struct Piece board[COL][ROW], enum Color color) {
	printf("\n");

	bool isWhite = (color == white);

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


/*** movegen ***/
/*** pseudolegality thanks to https://github.com/JDSherbert for some movegen logic ***/
// en passant is stupid :/
bool pawn_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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


bool knight_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
	if ((abs(x - pX) == 2) && (abs(y - pY) == 1) || (abs(x - pX) == 1) && (abs(x - pY) == 2)) return true;

	else return false;
}

bool bishop_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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

bool rook_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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

bool queen_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) {
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

bool king_rule(struct Piece board[ROW][COL], int x, int y, int pX, int pY) { 
	if (abs(x - pX) <= 1 && abs(y - pY) <= 1) return true;
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
				case pawn: isAttacked = pawn_rule(board, r, c, x, y); break;
				case knight: isAttacked = knight_rule(board, r, c, x, y); break;
				case bishop: isAttacked = bishop_rule(board, r, c, x, y); break;
				case rook: isAttacked = rook_rule(board, r, c, x, y); break;
				case queen: isAttacked = queen_rule(board, r, c, x, y); break;
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
		case empty: return false;
		default: return false;
	}
}

bool board_status(struct Piece board[ROW][COL], enum Color color_to_move, int x, int y, int pX, int pY) {
	if (king_attacked(board, color_to_move)) return true;

	struct Piece temp_board[ROW][COL];

	memcpy(temp_board, board, sizeof(struct Piece) * ROW * COL);

	move_piece(temp_board, x, y, pX, pY);
	
	if (king_attacked(temp_board, color_to_move)) return true;
	
	return false;
}

/*** perft ***/
unsigned long perft(struct Piece board[ROW][COL], int depth, enum Color color_to_move) {
	if (depth == 0) {
		return 1;    
	}

	unsigned long nodes = 0;

	// Loop through all pieces of the current player
	for (int x = 0; x < ROW; x++) {
		for (int y = 0; y < COL; y++) {

			if (board[x][y].color != color_to_move) continue;

			// Generate pseudo-legal moves for this piece
			for (int pX = 0; pX < ROW; pX++) {
				for (int pY = 0; pY < COL; pY++) {
					if (!check_move(board, x, y, pX, pY)) continue;

					if (board[x][y].color != color_to_move) continue;

					if (board[x][y].color == board[pX][pY].color) continue;

					if (board_status(board, color_to_move, x, y, pX, pY)) continue;

					// Save the pieces involved in the move
					struct Piece moving_piece = board[x][y];
					struct Piece captured_piece = board[pX][pY];

					char node_type = get_type_char(moving_piece.type);
					//printf("%c, x: %d, y: %d \n", node_type, pX, pY); 

					// Make the move
					move_piece(board, x, y, pX, pY);

					// Check legality (doesn't leave own king in check)
					nodes += perft(board, depth - 1, reverse_color(color_to_move));

					// Undo the move
					board[x][y] = moving_piece;
					board[pX][pY] = captured_piece;
				}
			}
		}
	}

	return nodes;
}

void test_perft(struct Piece board[ROW][COL], int depth, enum Color color_to_move) {
	clock_t start_time = clock();

	unsigned long nodes = perft(board, depth, color_to_move);

	clock_t end_time = clock();

	printf("Perft at depth %d: %lu nodes \n", depth, nodes);

	double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	printf("Time taken: %.3f seconds \n", time_spent);
}

/*** gameloop ***/
void game(struct Piece board[ROW][COL]) {
	int x;
	int y;
	int pX;
	int pY;


	// Check king castling
	bool is_rook1_white_moved = false;
	bool is_rook2_white_moved = false;
	bool castling_white = true;

	bool is_rook1_black_moved = false;
	bool is_rook2_black_moved = false;
	bool castling_black = true;

	
	enum Color color_to_move = white;

	int i;
	
	clear_scr();

	while (true) {
		test_perft(board, 4, color_to_move);

		print_board(board, color_to_move);

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

		if (board[x][y].color != color_to_move) {
			printf("Wrong color to move \n");
			continue;
		}

		if (board[x][y].color == board[pX][pY].color) {
			printf("Can't move your piece onto your own color");
			continue;
		}

		// castling shenanigans
		if (board[x][y].type == king) {
			if (x == pX && (color_to_move == white ? castling_white : castling_black)) {
				int rook_row = color_to_move == white ? 7 : 0;
				
				struct Piece temp_board[ROW][COL];
				memcpy(temp_board, board, sizeof(struct Piece) * ROW * COL);

				if (pY == 2 && (color_to_move == white ? !is_rook1_white_moved : !is_rook1_black_moved)) {
				    for (int w = y - 1; w >= pY; w--) {
					move_piece(board, x, w+1, pX, w);

					if (board[x][w].type != empty || king_attacked(board, color_to_move)) {
					    printf("Castling failed: obstructed or under attack.\n");
					    memcpy(board, temp_board, sizeof(struct Piece) * ROW * COL);

					    break;
					}

					if (w == pY) {
					    move_piece(board, rook_row, 0, rook_row, 3);
					    if (color_to_move == white) castling_white = false;
					    else castling_black = false;

					    color_to_move = reverse_color(color_to_move);
					}
				    }
				} else if (pY == 6 && (color_to_move == white ? !is_rook2_white_moved : !is_rook2_black_moved)) {
				    for (int w = y + 1; w <= pY; w++) {
					move_piece(board, x, w-1, pX, w);

					if (board[x][w].type != empty || king_attacked(board, color_to_move)) {
					    printf("Castling failed: obstructed or under attack.\n");
					    memcpy(board, temp_board, sizeof(struct Piece) * ROW * COL);

					    break;
					}

					if (w == pY) {
					    move_piece(board, rook_row, 7, rook_row, 5);
					    if (color_to_move == white) castling_white = false;
					    else castling_black = false;

					    color_to_move = reverse_color(color_to_move);
					}
				    }
				} else {
				    printf("Invalid castling move.\n");
				}
			}
		}


		if (check_move(board, x, y, pX, pY)) {
			if (board_status(board, color_to_move, x, y, pX, pY)) {
				printf("You're in check or moving into it. \n");
				continue;
			}

			move_piece(board, x, y, pX, pY);

			// Change castling rights
			if (color_to_move == white) {
				if (board[x][y].type == rook && y == 7) is_rook1_white_moved = true;
				else if (board[x][y].type == rook && y == 0) is_rook2_white_moved = true;
				else if (board[x][y].type == king) castling_white = false;
			} else {
				if (board[x][y].type == rook && y == 7) is_rook1_black_moved = true;
				else if (board[x][y].type == rook && y == 0) is_rook2_black_moved = true;
				else if (board[x][y].type == king) castling_black = false;
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
	setlocale(LC_ALL, "");
	struct Piece board[ROW][COL];
	set_board(STARTING_FEN, board);
	game(board);

	return 0;
}
