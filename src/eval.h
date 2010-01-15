/*  PurpleHaze 1.0
    Copyright (C) 2007-2009  Vincent Ollivier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EVAL_H
#define EVAL_H

#include <vector>

const int PAWN_PCSQ[BOARD_SIZE] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     -6,  -4,   1,   1,   1,   1,  -4,  -6,   0,   0,   0,   0,   0,   0,   0,   0,
     -6,  -4,   1,   2,   2,   1,  -4,  -6,   0,   0,   0,   0,   0,   0,   0,   0,
     -6,  -4,   2,   8,   8,   2,  -4,  -6,   0,   0,   0,   0,   0,   0,   0,   0,
     -6,  -4,   5,  10,  10,   5,  -4,  -6,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,  -4,   1,   5,   5,   1,  -4,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -6,  -4,   1, -24, -24,   1,  -4,  -6,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

const int KNIGHT_PCSQ[BOARD_SIZE] = {
     -8,  -8,  -8,  -8,  -8,  -8,  -8,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,   0,   0,   0,   0,   0,   0,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,   0,   4,   4,   4,   4,   0,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,   0,   4,   8,   8,   4,   0,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,   0,   4,   8,   8,   4,   0,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,   0,   4,   4,   4,   4,   0,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,   0,   1,   2,   2,   1,   0,  -8,   0,   0,   0,   0,   0,   0,   0,   0,
     -8,  -12, -8,  -8,  -8,  -8, -12,  -8,   0,   0,   0,   0,   0,   0,   0,   0
};

const int BISHOP_PCSQ[BOARD_SIZE] = {
     -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,   0,   0,   0,   0,   0,   0,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,   0,   2,   4,   4,   2,   0,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,   0,   4,   6,   6,   4,   0,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,   0,   4,   6,   6,   4,   0,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,   1,   2,   4,   4,   2,   1,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,   2,   1,   1,   1,   1,   2,  -4,   0,   0,   0,   0,   0,   0,   0,   0,
     -4,  -4, -12,  -4,  -4, -12,  -4,  -4,   0,   0,   0,   0,   0,   0,   0,   0
};

const int ROOK_PCSQ[BOARD_SIZE] = {
      5,   5,   5,   5,   5,   5,   5,   5,   0,   0,   0,   0,   0,   0,   0,   0,
     20,  20,  20,  20,  20,  20,  20,  20,   0,   0,   0,   0,   0,   0,   0,   0,
     -5,   0,   0,   0,   0,   0,   0,  -5,   0,   0,   0,   0,   0,   0,   0,   0,
     -5,   0,   0,   0,   0,   0,   0,  -5,   0,   0,   0,   0,   0,   0,   0,   0,
     -5,   0,   0,   0,   0,   0,   0,  -5,   0,   0,   0,   0,   0,   0,   0,   0,
     -5,   0,   0,   0,   0,   0,   0,  -5,   0,   0,   0,   0,   0,   0,   0,   0,
     -5,   0,   0,   0,   0,   0,   0,  -5,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

const int QUEEN_PCSQ[BOARD_SIZE] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   2,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   2,   3,   3,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   2,   3,   3,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   2,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,   0,   0,   0,   0,   0
};
 
const int KING_PCSQ[BOARD_SIZE] = {
    -40, -40, -40, -40, -40, -40, -40, -40,   0,   0,   0,   0,   0,   0,   0,   0,
    -40, -40, -40, -40, -40, -40, -40, -40,   0,   0,   0,   0,   0,   0,   0,   0,
    -40, -40, -40, -40, -40, -40, -40, -40,   0,   0,   0,   0,   0,   0,   0,   0,
    -40, -40, -40, -40, -40, -40, -40, -40,   0,   0,   0,   0,   0,   0,   0,   0,
    -40, -40, -40, -40, -40, -40, -40, -40,   0,   0,   0,   0,   0,   0,   0,   0,
    -40, -40, -40, -40, -40, -40, -40, -40,   0,   0,   0,   0,   0,   0,   0,   0,
    -15, -15, -20, -20, -20, -20, -15, -15,   0,   0,   0,   0,   0,   0,   0,   0,
      0,  20,  30, -30,   0, -20,  30,  20,   0,   0,   0,   0,   0,   0,   0,   0
};
 
const int KING_ENDGAME_PCSQ[BOARD_SIZE] = {
      0,  10,  20,  30,  30,  20,  10,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     10,  20,  30,  40,  40,  30,  20,  10,   0,   0,   0,   0,   0,   0,   0,   0,
     20,  30,  40,  50,  50,  40,  30,  20,   0,   0,   0,   0,   0,   0,   0,   0,
     30,  40,  50,  60,  60,  50,  40,  30,   0,   0,   0,   0,   0,   0,   0,   0,
     30,  40,  50,  60,  60,  50,  40,  30,   0,   0,   0,   0,   0,   0,   0,   0,
     20,  30,  40,  50,  50,  40,  30,  20,   0,   0,   0,   0,   0,   0,   0,   0,
     10,  20,  30,  40,  40,  30,  20,  10,   0,   0,   0,   0,   0,   0,   0,   0,
      0,  10,  20,  30,  30,  20,  10,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

const int WEAK_PAWN_PCSQ[BOARD_SIZE] = {
	  0,   0,    0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,
	-10, -12,  -14, -16, -16, -14, -12, -10,  0,   0,   0,   0,   0,   0,   0,   0,
	-10, -12,  -14, -16, -16, -14, -12, -10,  0,   0,   0,   0,   0,   0,   0,   0,
	-10, -12,  -14, -16, -16, -14, -12, -10,  0,   0,   0,   0,   0,   0,   0,   0,
	-10, -12,  -14, -16, -16, -14, -12, -10,  0,   0,   0,   0,   0,   0,   0,   0,
	 -8, -12,  -14, -16, -16, -14, -12, -10,  0,   0,   0,   0,   0,   0,   0,   0,
	 -8, -12,  -14, -16, -16, -14, -12, -10,  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,    0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0
};

const int PASSED_PAWN_PCSQ[BOARD_SIZE] = {
	  0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,
	100, 100, 100, 100, 100, 100, 100, 100,  0,   0,   0,   0,   0,   0,   0,   0,
	 80,  80,  60,  80,  80,  80,  80,  80,  0,   0,   0,   0,   0,   0,   0,   0,
	 60,  60,  60,  60,  60,  60,  60,  60,  0,   0,   0,   0,   0,   0,   0,   0,
	 40,  40,  40,  40,  40,  40,  40,  40,  0,   0,   0,   0,   0,   0,   0,   0,
	 20,  20,  20,  20,  20,  20,  20,  20,  0,   0,   0,   0,   0,   0,   0,   0,
	 20,  20,  20,  20,  20,  20,  20,  20,  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0
 };


const int FLIP[BOARD_SIZE] = {
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
     96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
     80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
     64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
     48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
     32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
     16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
/*
   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
  96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
*/
/*
  15,  14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,
  47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,
  63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48,
  79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64,
  95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80,
 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96,
 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112
*/
/*
 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112,
 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96,
  95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80,
  79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64,
  63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48,
  47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,
  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,
  15,  14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
*/
};

const int KNIGHT_ADJ[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 12};
const int ROOK_ADJ[9] =   {  15,  12,   9,  6,  3,  0, -3, -6, -9};


const int BONUS_CASTLE = 100;
const int BONUS_KING_SHIELD = 30;
const int BONUS_RANDOM_MAX = 3;
const int BONUS_BISHOP_PAIR = 30;
const int MALUS_NO_PAWN = -30;

const int MALUS_KING_BREAKING_CASTLE_RIGHT = -40;

const int MALUS_QUEEN_EARLY_MOVE = -20;

const int BONUS_ROOK_OPEN_FILE = 30;
const int BONUS_ROOK_SEVENTH_RANK = 5;

const int MALUS_MULTI_PAWN[7] = {0, 0, -25, -60, -100, -100, -100};

const int NB_TYPE = 6;
const int NB_FILE = 8;
const int NB_RANK = 8;

class Evaluation
{
	public:
		int nb_pieces[NB_TYPE];
		int nb_pieces_file[NB_TYPE][NB_FILE];
		int nb_pieces_rank[NB_TYPE][NB_RANK];
		Pieces& pieces;
		vector<Piece*> pawns;
		Color color, color_single_bishop;
		int lazy_score;
		int material_score;
		int positional_score;
		
		Evaluation(Pieces& p);
		void pre_build(Board& board);
		void build(Board& board);
		int get_lazy_eval();
		int king_eval(Board& board);
		int queens_eval(Board& board);
		int rooks_eval(Board& board, Evaluation opponent);
		int bishops_eval(Board& board);
		int knights_eval(Board& board);
		int pawns_eval(Board& board, Evaluation opponent);
};

int static_exchange_evaluation(Board board, Square square, Color player_color);
int eval(Board& board, Pieces& engine, Pieces& opponent);
bool is_in_check(Board& board, Pieces* ptr_pieces_player);
bool is_promoting_pawn(Board& board);

#endif /* !EVAL_H */
