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

#include <iostream>
#include "piece.h"

using namespace std;

Piece::Piece(Color c, PieceType t, int i) {
    code = (i << 4) | (int(t) << 1) | int(c);
}

ostream& operator<<(ostream& out, const Piece piece) {
    switch (piece.get_color()) {
	case WHITE:
	    switch (piece.get_type()) {
		case PAWN: out << "P"; break;
		case KNIGHT: out << "N"; break;
		case BISHOP: out << "B"; break;
		case ROOK: out << "R"; break;
		case QUEEN: out << "Q"; break;
		case KING: out << "K"; break;
		default: break;
	    }
	    break;
	case BLACK:
	    switch (piece.get_type()) {
		case PAWN: out << "p"; break;
		case KNIGHT: out << "n"; break;
		case BISHOP: out << "b"; break;
		case ROOK: out << "r"; break;
		case QUEEN: out << "q"; break;
		case KING: out << "k"; break;
		default: break;
	   }
    }
    return out;
}
