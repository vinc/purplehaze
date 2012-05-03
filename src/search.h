/* Copyright (C) 2007-2011 Vincent Ollivier
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

#ifndef SEARCH_H
#define SEARCH_H

#include "common.h"
#include "eval.h"

enum NodeType : unsigned char { PV, CUT, ALL };

// Adaptive Null-Move Pruning (Heinz 1999)
inline static int R_ADAPT(int d, int n)
{
    return 2 + (d > (6 + ((n < 3) ? 2 : 0)));
}

static const int R = 2;
static const int NMP_DEPTH = 1;      // depth > NMP_DEPTH
static const int LMR_DEPTH = 2;      // depth > LMR_DEPTH
static const int IID_DEPTH = 3;      // depth > IID_DEPTH
static const int FUTILITY_DEPTH = 3; // depth <= FUTILITY_DEPTH

// Array of pruning margin values indexed by depth (idea from Crafty)
static const int FUTILITY_MARGINS[] = {
     0 * PIECE_VALUE[PAWN],
    13 * PIECE_VALUE[PAWN], // FIXME: Margins found after 200k+ games, but
    10 * PIECE_VALUE[PAWN], // those results are in contradiction with the
    10 * PIECE_VALUE[PAWN]  // theory.
};

inline static int value_to_trans(int value, int ply)
{
    if (value < -INF + MAX_PLY) {
        value -= ply;
    } else if (value > INF - MAX_PLY) {
        value += ply;
    }
    return value;
}

inline static int value_from_trans(int value, int ply)
{
    if (value < -INF + MAX_PLY) {
        value += ply;
    } else if (value > INF - MAX_PLY) {
        value -= ply;
    }
    return value;
}

#endif /* !SEARCH_H */
