/*** dependencies ***/
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
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"
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
		case empty: printf("Cannot move an empty square \n"); return false;
		default: return false;
	}
}

/*** perft ***/
void perft(struct Piece board[ROW][COL]) {
	int pos_searched = 0;

	for (int x = 0; x < COL; x++) {
		for (int y = 0; y < ROW; y++) {
			enum Type type = board[x][y].type;

			if (type == empty) continue;

			for (int pX = 0; pX < COL; pX++) {
				for (int pY = 0; pY < ROW; pY++) {
					pos_searched++;

					switch (type) {
						case pawn: if(!pawn_rule(board, x, y, pX, pY)) continue; break;
						case knight: if(!knight_rule(board, x, y, pX, pY)) continue; break;
						case bishop: if(!bishop_rule(board, x, y, pX, pY)) continue; break;
						case rook: if(!rook_rule(board, x, y, pX, pY)) continue; break;
						case queen: if(!queen_rule(board, x, y, pX, pY)) continue; break;
						case king: if(!king_rule(board, x, y, pX, pY)) continue; break;
					}

					if (board[x][y].color == board[pX][pY].color) continue;

					char type_char = get_type_char(type);

					printf("%c | %c %d, %c %d \n", type_char, y + 'a', x + 1, pY + 'a', pX + 1);
				}
			}
		}
	}

	printf("\n Searched %d positions! \n\n", pos_searched);
}

/*** gameloop ***/
void game(struct Piece board[ROW][COL]) {
	int x;
	int y;
	int pX;
	int pY;


	// Check king castling
	bool is_king_moved = false;
	bool is_rook1_moved = false;
	bool is_rook2_moved = false;
	bool castling = false;


	enum Color color_to_move = white;

	int i;
	
	clear_scr();

	while (true) {
		print_board(board, color_to_move);

		char from[3], to[3];

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

		if (board[x][y].color == board[pX][pY].color) {
			printf("Can't move your piece onto your own color");
			continue;
		}

		if (x == pX && !is_king_moved) {
			if ((y - pY == -2) && !is_rook2_moved) castling = true;
			if ((y - pY == 2) && !is_rook1_moved) castling = true;
		}


		if (check_move(board, x, y, pX, pY) || castling) {
			struct Piece temp_board[ROW][COL];
			enum Type type = board[x][y].type == king;

			memcpy(temp_board, board, sizeof(struct Piece) * ROW * COL);

			move_piece(board, x, y, pX, pY);
			
			if (king_attacked(board, color_to_move)) {
				printf("Your king is in check \n");
				memcpy(board, temp_board, sizeof(struct Piece) * ROW * COL);
				castling = false;
				continue;
			} else if (type == king) {
				is_king_moved = true;
			} else if (type == rook) {
				if (y == 0) is_rook1_moved = true;
				if (y == 7) is_rook2_moved = true;
			}

			color_to_move = reverse_color(color_to_move);
		} else {
			printf("Unable to move that piece there");
		}

		printf("\n");
		
		//perft(board);
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
