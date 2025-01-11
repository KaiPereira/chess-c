#include "positions.h"


// For efficiency purposes I reverse the position already

// Pawn
int pawn_table_b[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{5, 10, 10, -20, -20, 10, 10, 5},
	{5, -5, -10, 0, 0, -10, -5, 5},
	{0, 0, 0, 20, 20, 0, 0, 0},
	{5, 5, 10, 25, 25, 10, 5, 5},
	{10, 10, 20, 30, 30, 20, 10, 10},
	{50, 50, 50, 50, 50, 50, 50, 50},
	{0, 0, 0, 0, 0, 0, 0, 0}
};

int pawn_table_w[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{50, 50, 50, 50, 50, 50, 50, 50},
	{10, 10, 20, 30, 30, 20, 10, 10},
	{5, 5, 10, 25, 25, 10, 5, 5},
	{0, 0, 0, 20, 20, 0, 0, 0},
	{5, -5, -10, 0, 0, -10, -5, 5},
	{5, 10, 10, -20, -20, 10, 10, 5},
	{0, 0, 0, 0, 0, 0, 0, 0}
};

// Knight
int knight_table_b[8][8] = {
	{-50, -40, -30, -30, -30, -30, -40, -50},
	{-40, -20, 0, 5, 5, 0, -20, -40},
	{-30, 5, 10, 15, 15, 10, 5, -30},
	{-30, 0, 15, 20, 20, 15, 0, -30},
	{-30, 5, 15, 20, 20, 15, 5, -30},
	{-30, 0, 10, 15, 15, 10, 0, -30},
	{-40, -20, 0, 0, 0, 0, -20, -40},
	{-50, -40, -30, -30, -30, -30, -40, -50}
};

int knight_table_w[8][8] = {
	{-50, -40, -30, -30, -30, -30, -40, -50},
	{-40, -20, 0, 0, 0, 0, -20, -40},
	{-30, 0, 10, 15, 15, 10, 0, -30},
	{-30, 5, 15, 20, 20, 15, 5, -30},
	{-30, 0, 15, 20, 20, 15, 0, -30},
	{-30, 5, 10, 15, 15, 10, 5, -30},
	{-40, -20, 0, 5, 5, 0, -20, -40},
	{-50, -40, -30, -30, -30, -30, -40, -50}
};

// Bishop
int bishop_table_b[8][8] = {
	{-20, -10, -10, -10, -10, -10, -10, -20},
	{-10, 5, 0, 0, 0, 0, 5, -10},
	{-10, 10, 10, 10, 10, 10, 10, -10},
	{-10, 0, 10, 10, 10, 10, 0, -10},
	{-10, 5, 5, 10, 10, 5, 5, -10},
	{-10, 0, 5, 10, 10, 5, 0, -10},
	{-10, 0, 0, 0, 0, 0, 0, -10},
	{-20, -10, -10, -10, -10, -10, -10, -20}
};

int bishop_table_w[8][8] = {
	{-20, -10, -10, -10, -10, -10, -10, -20},
	{-10, 0, 0, 0, 0, 0, 0, -10},
	{-10, 0, 5, 10, 10, 5, 0, -10},
	{-10, 5, 5, 10, 10, 5, 5, -10},
	{-10, 0, 10, 10, 10, 10, 0, -10},
	{-10, 10, 10, 10, 10, 10, 10, -10},
	{-10, 5, 0, 0, 0, 0, 5, -10},
	{-20, -10, -10, -10, -10, -10, -10, -20}
};

// Rook
int rook_table_b[8][8] = {
	{0, 0, 0, 5, 5, 0, 0, 0},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{5, 10, 10, 10, 10, 10, 10, 5},
	{0, 0, 0, 0, 0, 0, 0, 0}
};

int rook_table_w[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{5, 10, 10, 10, 10, 10, 10, 5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{-5, 0, 0, 0, 0, 0, 0, -5},
	{0, 0, 0, 5, 5, 0, 0, 0}
};

// Queen
int queen_table_b[8][8] = {
	{-20, -10, -10, -5, -5, -10, -10, -20},
	{-10, 0, 0, 0, 0, 0, 0, -10},
	{-10, 5, 5, 5, 5, 5, 0, -10},
	{0, 0, 5, 5, 5, 5, 0, -5},
	{-5, 0, 5, 5, 5, 5, 0, -5},
	{-10, 0, 5, 5, 5, 5, 0, -10},
	{-10, 0, 0, 0, 0, 0, 0, -10},
	{-20, -10, -10, -5, -5, -10, -10, -20}
};

int queen_table_w[8][8] = {
	{-20, -10, -10, -5, -5, -10, -10, -20},
	{-10, 0, 0, 0, 0, 0, 0, -10},
	{-10, 0, 5, 5, 5, 5, 0, -10},
	{-5, 0, 5, 5, 5, 5, 0, -5},
	{0, 0, 5, 5, 5, 5, 5, -5},
	{-10, 5, 5, 5, 5, 5, 0, -10},
	{-10, 0, 0, 0, 0, 0, 0, -10},
	{-20, -10, -10, -5, -5, -10, -10, -20}
};

// King
int king_table_b[8][8] = {
	{20, 40, -5, 0, -5, -5, 40, 20},
	{20, 20, -5, -5, -5, -5, 20, 20},
	{-10, -20, -20, -20, -20, -20, -20, -10},
	{-20, -30, -30, -40, -40, -30, -30, -20},
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-30, -40, -40, -50, -50, -40, -40, -30}
};

int king_table_w[8][8] = {
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-30, -40, -40, -50, -50, -40, -40, -30},
	{-20, -30, -30, -40, -40, -30, -30, -20},
	{-10, -20, -20, -20, -20, -20, -20, -10},
	{20, 20, -5, -5, -5, -5, 20, 20},
	{20, 40, -5, 0, -5, -5, 40, 20}
};
