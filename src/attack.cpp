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

#include "game.h"

bool Board::is_attacked_by(Color c, Square s, const Pieces& pieces) const {
    for (PieceType t = KNIGHT; t <= KING; t = PieceType(t + 1)) {
	int n = pieces.get_nb_pieces(c, t);
	for (int i = 0; i < n; ++i) {
	    Square from = pieces.get_position(c, t, i);
	    if (!can_attack(t, from, s)) continue;
	    if (t == KNIGHT || t == KING) return true;
	    Direction d = get_direction_to(from, s);
	    Square to = Square(from + d);
	    while (to != s && is_empty(to)) {
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
	if (get_piece(from).get_type() == PAWN &&
	    get_piece(from).get_color() == c) {
	    return true;
	}
    }

    return false;
}

bool Board::can_go(Piece p, Square from, Square to) const {
    PieceType t = p.get_type();
    Color c = p.get_color();
    Direction d = get_direction_to(from, to);
    
    // A piece cannot capture another piece of the same color
    if (!is_empty(to) && get_piece(to).get_color() == c) return false;
    
    Direction push_dir;
    Square s;
    switch (t) {
	case PAWN:
	    push_dir = (c == WHITE ? UP : DOWN);
	    if (!is_empty(to)) {
		if (to == Square(from + push_dir + LEFT)) return true;
		if (to == Square(from + push_dir + RIGHT)) return true;
	    }
	    else {
		if (to == Square(from + push_dir)) return true;
		if (to == Square(from + 2 * push_dir) &&
		    is_pawn_begin(c, from)) return true;
	    }
	    break;
	default:
	    if (!can_attack(t, from, to)) return false;
	    if (t == KNIGHT || t == KING) return true;
	    s = Square(from + d);
	    while (s != to && is_empty(s)) {
		s = Square(s + d);
	    }
	    if (s == to) return true;
	    break;
    }
    return false;
}

