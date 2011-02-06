/* PurpleHaze 2.0.0
 * Copyright (C) 2007-2011  Vincent Ollivier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

#include <list>
#include <vector>

#define BOARD_SIZE    128
#define MAX_DEPTH     128
#define INF	    29999

enum Square : unsigned char {
    A1=0x00, B1, C1, D1, E1, F1, G1, H1,
    A2=0x10, B2, C2, D2, E2, F2, G2, H2,
    A3=0x20, B3, C3, D3, E3, F3, G3, H3,
    A4=0x30, B4, C4, D4, E4, F4, G4, H4,
    A5=0x40, B5, C5, D5, E5, F5, G5, H5,
    A6=0x50, B6, C6, D6, E6, F6, G6, H6,
    A7=0x60, B7, C7, D7, E7, F7, G7, H7,
    A8=0x70, B8, C8, D8, E8, F8, G8, H8,
    OUT
};

enum File : unsigned char { 
    FILE_A, 
    FILE_B, 
    FILE_C, 
    FILE_D, 
    FILE_E, 
    FILE_F, 
    FILE_G, 
    FILE_H 
};

enum Rank : unsigned char { 
    RANK_1, 
    RANK_2, 
    RANK_3, 
    RANK_4, 
    RANK_5, 
    RANK_6, 
    RANK_7, 
    RANK_8 
};

enum Direction {
	NO_DIR		    = 0,
	UP		    = 0x10,
	RIGHT		    = 0x01,
	DOWN		    = -UP,
	LEFT		    = -RIGHT,
	UP_RIGHT	    = UP + RIGHT,
	DOWN_RIGHT	    = DOWN + RIGHT,
	DOWN_LEFT	    = DOWN + LEFT,
	UP_LEFT		    = UP + LEFT,
	UP_UP_RIGHT	    = UP + UP_RIGHT,
	RIGHT_UP_RIGHT	    = RIGHT + UP_RIGHT,
	RIGHT_DOWN_RIGHT    = RIGHT + DOWN_RIGHT,
	DOWN_DOWN_RIGHT	    = DOWN + DOWN_RIGHT,
	DOWN_DOWN_LEFT	    = DOWN + DOWN_LEFT,
	LEFT_DOWN_LEFT	    = LEFT + DOWN_LEFT,
	LEFT_UP_LEFT	    = LEFT + UP_LEFT,
	UP_UP_LEFT	    = UP + UP_LEFT
};

enum Color : bool { 
    WHITE, 
    BLACK
};

enum PieceType : unsigned char { 
    EMPTY,
    PAWN, 
    KNIGHT, 
    BISHOP, 
    ROOK, 
    QUEEN, 
    KING
};

enum MoveType : unsigned char { 
    QUIET_MOVE, 
    DOUBLE_PAWN_PUSH,
    KING_CASTLE,
    QUEEN_CASTLE,
    CAPTURE,
    EN_PASSANT,
    NULL_MOVE,		    // warning: is_capture() will return true
    KNIGHT_PROMOTION = 8,   // is_promotion() { MoveType[3] }
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};

// Used in movegen.cpp and attack.cpp
const int NB_DIRS[] = { 0, 0, 8, 4, 4, 8, 8 };
/*
const Direction NO_DIRS[8] = {
    NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR
};
const Direction KNIGHT_DIRS[8] = {
    UP_UP_RIGHT, RIGHT_UP_RIGHT, RIGHT_DOWN_RIGHT, DOWN_DOWN_RIGHT, 
    DOWN_DOWN_LEFT, LEFT_DOWN_LEFT, LEFT_UP_LEFT, UP_UP_LEFT
};
const Direction BISHOP_DIRS[8] = {
    UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT, NO_DIR, NO_DIR, NO_DIR, NO_DIR
};
const Direction ROOK_DIRS[8] = {
    UP, RIGHT, DOWN, LEFT, NO_DIR, NO_DIR, NO_DIR, NO_DIR
};
const Direction QUEEN_DIRS[8] = {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
};
const Direction KING_DIRS[8] = {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
};
*/
const Direction PIECES_DIRS[][8] = {
    {
	NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
	NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
	UP_UP_RIGHT, RIGHT_UP_RIGHT, RIGHT_DOWN_RIGHT, DOWN_DOWN_RIGHT, 
	DOWN_DOWN_LEFT, LEFT_DOWN_LEFT, LEFT_UP_LEFT, UP_UP_LEFT
    },
    {
	UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT, 
	NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
	UP, RIGHT, DOWN, LEFT, NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
	UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
    },
    {
	UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
    }
};

// Used in movegen.cpp and protocol.cpp
const Direction PAWN_PUSH_DIRS[2] = { UP, DOWN };
const Direction PAWN_CAPTURE_DIRS[2][2] = {
    {UP_LEFT, UP_RIGHT},
    {DOWN_LEFT, DOWN_RIGHT}
};

#define assert_msg(x) !(std::cerr << "Assertion failed: " << x << std::endl)

#endif /* !COMMON_H */
