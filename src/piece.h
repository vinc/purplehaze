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

#ifndef PIECE_H
#define PIECE_H

#include <bitset>
#include <string>

#include "common.h"

using namespace std;

class Piece
{
    friend ostream& operator<<(ostream& out, const Piece piece);
    private:
	    /*
	     * A piece is coded using 8 bits:
	     *     1 bit for the color
	     *     3 bits for the type
	     *     4 bits for the index in pieces[c][t][i]
	     *
	     *	   FIXME? No warning given if index is > 4 bits
	     */
	    bitset<8> code;
	    //Piece(bitset<8> c) : code(c) {}
    public:
	    Piece(Color c = WHITE, PieceType t = EMPTY, int i = 0);
	    Color get_color() const { 
	        return Color(int(code[0])); 
	    };
	    PieceType get_type() const { 
	        return PieceType(((code << 4) >> 5).to_ulong()); 
	    };
	    int get_index() const { 
	        return (code >> 4).to_ulong();
	    };
	    void set_index(int i) { 
	        (code &= 0x0F) |= (i << 4); 
	    }
	    bool operator==(const Piece& other) const {
		return this->code == other.code;
	    }
	    bool operator!=(const Piece& other) const {
		return !(*this == other);
	    }
	    string to_string() const;
};

#endif /* !PIECE_H */
