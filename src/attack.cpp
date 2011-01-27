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
//#include <iostream>
#include <list>

#include "game.h"

//using namespace std;

bool Game::is_attacked_by(Color c, Square s) const {
    for (PieceType t = KNIGHT; t <= KING; t = PieceType(t + 1)) {
	int n = pieces.get_nb_pieces(c, t);
	for (int i = 0; i < n; ++i) {
	    Square from = pieces.get_position(c, t, i);
	    if (!can_attack(t, from, s)) continue;
	    if (t == KNIGHT || t == KING) return true;
	    Direction d = get_direction_to(from, s);
	    Square to = Square(from + d);
	    while (to != s && board.is_empty(to)) {
		to = Square(to + d);
	    }
	    if (to == s) return true;
	}
    }
    
    // Pawns
    Direction d = (c == WHITE ? DOWN : UP);
    Direction dirs[2] = { Direction(d + LEFT), Direction(d + RIGHT) };
    for (int i = 0; i < 2; ++i) {
	Square from = Square(s + dirs[i]);
	if (board.get_piece(from).get_type() == PAWN &&
	    board.get_piece(from).get_color() == c) {
	    return true;
	}
    }

    return false;
}

