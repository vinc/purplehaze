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
#include <iostream>

#include "move.h"
#include "piece.h"

using namespace std;

/*
Move::Move(Square o, Square d, MoveType t) {

}
*/

PieceType Move::get_promotion_type() const {
    switch (get_type()) {
	case KNIGHT_PROMOTION:
	case KNIGHT_PROMOTION_CAPTURE:
	    return KNIGHT;
	case BISHOP_PROMOTION:
	case BISHOP_PROMOTION_CAPTURE:
	    return BISHOP;
	case ROOK_PROMOTION:
	case ROOK_PROMOTION_CAPTURE:
	    return ROOK;
	case QUEEN_PROMOTION:
	case QUEEN_PROMOTION_CAPTURE:
	    return QUEEN;
	default:
	    return EMPTY;
    }
}
PieceType Move::get_castle_side() const {
    switch (get_type()) {
	case KING_CASTLE: return KING;
	case QUEEN_CASTLE: return QUEEN;
	default: return EMPTY;
    }
}

ostream& operator<<(ostream& out, const Move move) {
    return (out << move.to_string());
}

string Move::to_string() const {
    if (is_null()) return "#";
    string res = "";
    res += char('a' + get_orig_file());
    res += char('1' + get_orig_rank());
    res += char('a' + get_dest_file());
    res += char('1' + get_dest_rank());
    if (is_promotion()) {
	res += Piece(BLACK, get_promotion_type()).to_string(); // Lower case
    }
    return res;
}
