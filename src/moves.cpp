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

#include <assert.h>

#include "moves.h"
#include "eval.h" // For PIECE_VALUES[]

/*
Moves::Moves() {
    
}
*/

void Moves::sort(Board b, Move bm, Move killer1, Move killer2) {
    for (int i = 0; i != size(); ++i) {
	ExtendedMove& m = moves[i];
	//if (m.get_orig() == bm.get_orig() &&
	//    m.get_dest() == bm.get_dest() &&
	//    m.get_type() == bm.get_type()) {
	if (m == bm) m.set_score(127);
	else if (m == killer1) m.set_score(1);
	else if (m == killer2) m.set_score(0);
	else if (m.is_capture()) {
	    PieceType a = b.get_piece(m.get_orig()).get_type();
	    PieceType v = b.get_piece(m.get_dest()).get_type();
	    int aggressor = PIECE_VALUE[a];
	    int victim = PIECE_VALUE[v];
	    if (m.is_en_passant()) victim = PIECE_VALUE[PAWN];
	    int score = ((victim - aggressor) / 16);
	    //assert(PIECE_VALUE[KING] == 10000);
	    if (v == KING) score -= 500;
	    if (a == KING) score += 500;
	    m.set_score(score);
	}
	else {
	    m.set_score(-127); // SCORE_QUIET;
	}
    }
    selection_sort();
}

void Moves::selection_sort() {
    // Algorithm directly taken from Wikipedia
    int min;
    // Advance the position through the entire array
    for (int pos = 0; pos < n; ++pos) {
    // Find the min element in the unsorted moves[pos .. n-1]
    // Assume min is the first element
    min = pos;
    // Test against all other elements
    for (int i = pos + 1; i < n; ++i)	{
	// If this element is less, then it is the new minimum  
	if (moves[i] < moves[min]) {
	    // Found a new minimum; remember its index
	    min = i;
	}
    }
    // min is the index of the minimum element. 
    // Swap it with the current position
    ExtendedMove tmp = moves[pos];
    moves[pos] = moves[min];
    moves[min] = tmp;
    }
}
