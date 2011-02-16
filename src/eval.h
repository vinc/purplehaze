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

#ifndef EVAL_H
#define EVAL_H

#include "common.h"

static const int PIECE_VALUE[] = { 
        0, // Empty piece
      100, // Pawn
      325, // Knight
      325, // Bishop
      500, // Rook
      965, // Queen (Rook + Minor + Pawn + Bishop Pair)
    10000, // King
};

// Adjustement based on the number of pawns
static const int PAWNS_ADJUSTEMENT[][9] = {
    {     0,   0,   0,   0,   0,   0,   0,   0,   0 }, // Empty
    {     0,   0,   0,   0,   0,   0,   0,   0,   0 }, // Pawn
    {	-30, -24, -18, -12,  -6,   0,   6,  12,  18 }, // Knight
    {	-15, -12,  -9,  -6,  -3,   0,   3,   6,   9 }, // Bishop
    {    60,  48,  36,  24,  12,   0, -12, -24, -36 }, // Rook
    {    30,  24,  18,  12,   6,   0,   0,   0,   0 }, // Queen
    {     0,   0,   0,   0,   0,   0,   0,   0,   0 }  // King
};

static const int MULTI_PAWNS_MALUS[] = { // Pawns on the same file
    0,   -5,  -50,  -75, -100, -100, -100
};

// Specific bonus and malus
static const int CASTLE_BONUS          = 100;
static const int BISHOP_PAIR_BONUS     =  40;
static const int OPEN_FILE_BONUS       =  30;
static const int HALF_OPEN_FILE_BONUS  =  15;
static const int REDUNDANCY_MALUS      = -10;
static const int BREAKING_CASTLE_MALUS = -40;
static const int NO_PAWNS_MALUS        = -50;


enum Phase : unsigned char { OPENING, ENDING };

static const int PHASE_COEF[] = { 0, 1, 2, 2, 4, 8, 0 };
static const int PHASE_MAX = 16 * PHASE_COEF[PAWN] + 
		       4 * PHASE_COEF[KNIGHT] +
		       4 * PHASE_COEF[BISHOP] +
		       4 * PHASE_COEF[ROOK] +
		       2 * PHASE_COEF[QUEEN]; // 64

// PST[Phase][Color][PieceType][Square]

static const int BORDER_MALUS = -5;
static const int CENTER_BONUS[] = {   
	0,   0,   1,   2,   2,   1,   0,   0
};
static const int PAWN_FILES_VALUES[]   = { 
      -15,  -5,   1,   5,   5,   1,  -5, -15
};
static const int OPENING_RANKS_BONUS[][8] {
    {   0,   0,   0,   0,   0,   0,   0,   0 }, // Dummy for PieceType EMPTY
    {   0, -10,  -5,   5,   4,   3,   0,   0 }, // Pawns
    {   0,   1,   2,   4,   4,   2,   0,   0 }, // Knights
    {   0,   1,   2,   3,   3,   2,   0,   0 }, // Bishops
    {  30,   0,   0,   0,   0,   0,   0,   0 }, // Rooks
    {  -5,   1,   2,   3,   3,   2,   1,   0 }, // Queens 
    {   0, -15, -40, -40, -40, -40, -40, -40,}  // King 
};

#endif /* !EVAL_H */
