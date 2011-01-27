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

#include "moves.h"

/*
Moves::Moves() {
    
}
*/

void Moves::sort(Move bm) { 
    for (int i = 0; i != size(); ++i) {
	if (moves[i].get_orig() == bm.get_orig() &&
	    moves[i].get_dest() == bm.get_dest() &&
	    moves[i].get_type() == bm.get_type()) {
	    moves[i].set_score(3); //SCORE_BEST_MOVE;
	}
	else if (moves[i].is_capture()) {
	    moves[i].set_score(2); //SCORE_CAPTURE;
	}
	else {
	    moves[i].set_score(1); // SCORE_QUIET;
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
