/* Copyright (C) 2007-2012 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EVAL_H
#define EVAL_H

#include "common.h"

// Specific bonus and malus
static const int CASTLE_BONUS          =  64;
static const int BISHOP_PAIR_BONUS     =  32;
static const int OPEN_FILE_BONUS       =  64;
static const int HALF_OPEN_FILE_BONUS  =  32;
static const int REDUNDANCY_MALUS      = -16;
static const int BREAKING_CASTLE_MALUS = -32;
static const int NO_PAWNS_MALUS        = -64;

static const int PIECE_VALUE[] = {
    0, // Empty piece
    100, // Pawn
    325, // Knight
    325, // Bishop
    500, // Rook
    965, // Queen (Rook + Minor + Pawn + Bishop Pair)
    10000, // King
};

// Pieces values adjustments based on the number of pawns
static const int PAWNS_ADJUSTEMENT[][9] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // Empty
    {   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // Pawns
    { -30, -24, -18, -12,  -6,   0,   6,  12,  18 }, // Knights
    { -15, -12,  -9,  -6,  -3,   0,   3,   6,   9 }, // Bishops
    {  60,  48,  36,  24,  12,   0, -12, -24, -36 }, // Rooks
    {  30,  24,  18,  12,   6,   0,   0,   0,   0 }, // Queens
    {   0,   0,   0,   0,   0,   0,   0,   0,   0 }  // King
};

static const int MULTI_PAWNS_MALUS[] = { // Pawns on the same file
    0,   -5,  -50,  -75, -100, -100, -100
};

// Game phase used in tapered evaluation
enum Phase : uint8_t { OPENING, ENDING };
static const Phase PHASES[] = { OPENING, ENDING };

// Coefficient of each type of piece to determine the game phase
static const int PHASE_COEF[] = { 0, 1, 2, 2, 4, 8, 0 };

// Opening number
static const int PHASE_MAX = 16 * PHASE_COEF[PAWN] +
                             4 * PHASE_COEF[KNIGHT] +
                             4 * PHASE_COEF[BISHOP] +
                             4 * PHASE_COEF[ROOK] +
                             2 * PHASE_COEF[QUEEN]; // == 64

// PST parameters
static const int BORDER_MALUS = -5;
static const int CENTER_BONUS[] = {
    0,   0,   1,   2,   2,   1,   0,   0
};
static const int PAWN_FILES_VALUES[] = {
    -16,  -4,   0,   8,   8,   0,  -4, -16
};
static const int OPENING_RANKS_BONUS[][8] = {
    {  0,   0,   0,   0,   0,   0,   0,   0 }, // Dummy for PieceType EMPTY
    {  0, -10,  -5,   5,   4,   3,   0,   0 }, // Pawns
    {  0,   1,   2,   4,   4,   2,   0,   0 }, // Knights
    {  0,   1,   2,   3,   3,   2,   0,   0 }, // Bishops
    { 30,   0,   0,   0,   0,   0,   0,   0 }, // Rooks
    { -5,   1,   2,   3,   3,   2,   1,   0 }, // Queens
    {  0, -15, -40, -40, -40, -40, -40, -40,}  // King
};

inline static bool has_bishop_pair(const Color c, const Pieces& pieces)
{
    assert(pieces.count(c, BISHOP) == 2);
    const Square fb = pieces.position(c, BISHOP, 0);
    const Square sb = pieces.position(c, BISHOP, 1);
    return !Board::is_same_color(fb, sb);
}

#endif /* !EVAL_H */
