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

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

using namespace std;

#define DEBUG 1
//#define SAN_OUTPUT 1
//#define RANDOM_EVAL 1
#define OPENING_BOOK 1
#define CHECK_EXTENSION 1
#define QUIESCENCE_SEARCH 1
#define DELTA_PRUNING 1 // Work well for a very little improvement
#define MVV_LVA
//#define SEE 1
//#define KILLER_HEURISTIC // FIXME Bad for perf
#define TRANSPOSITIONS_TABLE 1
//#define NULL_MOVE_PRUNING 1 // Increase size of tree
//#define LATE_MOVE_REDUCTION 1 // FIXME Have a serious bug. FIXME Not anymore?


/*
 * Generic datas
 */

const short INF = 29999;

const int ASCII_1 = int('1');
const int ASCII_8 = int('8');
const int ASCII_A = int('a');
const int ASCII_H = int('h');


enum Color { WHITE, BLACK, UNDEF_COLOR };

/*
 * Datas for representing the board
 */

static const int BOARD_SIZE = 128;

enum Square {
	A1=0x00, B1, C1, D1, E1, F1, G1, H1,
	A2=0x10, B2, C2, D2, E2, F2, G2, H2,
	A3=0x20, B3, C3, D3, E3, F3, G3, H3,
	A4=0x30, B4, C4, D4, E4, F4, G4, H4,
	A5=0x40, B5, C5, D5, E5, F5, G5, H5,
	A6=0x50, B6, C6, D6, E6, F6, G6, H6,
	A7=0x60, B7, C7, D7, E7, F7, G7, H7,
	A8=0x70, B8, C8, D8, E8, F8, G8, H8,
	OUT=0x89
};

enum File { A_FILE, B_FILE, C_FILE, D_FILE, E_FILE, F_FILE, G_FILE, H_FILE };

/*
 * Datas for representing the pieces
 */

enum PieceType {
	PAWN, 
	KNIGHT, 
	BISHOP, 
	ROOK, 
	QUEEN, 
	KING,
	UNDEF_PIECE_TYPE
};

enum PieceValue {
	UNDEF_PIECE_VALUE = 0,
	PAWN_VALUE = 100, 
	KNIGHT_VALUE = 320, 
	BISHOP_VALUE = 333, 
	ROOK_VALUE = 510, 
	QUEEN_VALUE = 880, 
	KING_VALUE = 10000	
};

/*
 * Datas for representing the moves
 */

enum MoveType { UNDEF_MOVE_TYPE, MOVE, CAPTURE, CASTLE, EN_PASSANT };

enum MoveOrientation {
	NO_MOVE				= 0,
	UP 					= 0x10,
	RIGHT 				= 0x01,
	DOWN 				= -UP,
	LEFT 				= -RIGHT,
	UP_RIGHT 			= UP + RIGHT,
	DOWN_RIGHT			= DOWN + RIGHT,
	DOWN_LEFT 			= DOWN + LEFT,
	UP_LEFT 			= UP + LEFT,
	UP_UP_RIGHT 		= UP + UP_RIGHT,
	RIGHT_UP_RIGHT		= RIGHT + UP_RIGHT,
	RIGHT_DOWN_RIGHT	= RIGHT + DOWN_RIGHT,
	DOWN_DOWN_RIGHT		= DOWN + DOWN_RIGHT,
	DOWN_DOWN_LEFT		= DOWN + DOWN_LEFT,
	LEFT_DOWN_LEFT		= LEFT + DOWN_LEFT,
	LEFT_UP_LEFT		= LEFT + UP_LEFT,
	UP_UP_LEFT			= UP + UP_LEFT
};

/*
 * Position in moves or pieces collections
 */
 
enum Position { UNDEF_POSITION, FRONT, BACK };

enum MoveScore { 
	SCORE_NORMAL_MOVE = 1,
	SCORE_KILLER_MOVE = 3,	
	SCORE_PROMOTION_MOVE = 4,
	SCORE_CAPTURE_MOVE = 6, 
	SCORE_BEST_MOVE = 10 
};

/*
 * Datas for representing the algos
 */

enum SearchAlgo { UNDEF_ALGO, NEGAMAX, ALPHABETA, PVS };

const int MAX_DEPTH = 100;
const int MAX_THINKING_TIME = 60;
const int REDUCED_DEPTH = 3; // Null move pruning

/*
 * Datas for Zobrist
 */

typedef uint64_t Hash;
//typedef unsigned long int Hash;

/*
 * Datas for Transposition
 */

enum Bound { EXACT, LOWER, UPPER, UNDEF_BOUND };

//TODO compare 1048576 and 1000000

//const int TT_SIZE = 1000000;
const int TT_SIZE = 1024*1024*128; // Transpositions table of 128Mb
const bool TT_STORE_CUTOFF = true;

#endif /* !COMMON_H */
