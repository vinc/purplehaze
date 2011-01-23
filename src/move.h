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

#ifndef MOVE_H
#define MOVE_H

#include <bitset>

#include "common.h"

using namespace std;

class Move {
    friend ostream& operator<<(ostream& out, const Move move);
    private:
	/*
	 * A move is coded using 16 bits:
	 *     4 bits for the type
	 *     3 bits for the destination square rank
	 *     3 bits for the destination square file 
	 *     3 bits for the origin square rank
	 *     3 bits for the origin square file 
	 */
	bitset<16> code;
	Move(bitset<16> c) : code(c) {}
    public:
	Move(Square o = A1, Square d = A1, MoveType t = NULL_MOVE);
	File get_orig_file() const {
	    return File((code >> 13).to_ulong());
	};
	Rank get_orig_rank() const {
	    return Rank(((code << 3) >> 13).to_ulong());
	};
	Square get_orig() const {
	    //return Square(16 * get_orig_rank() + get_orig_file());
	    return Square(16 * ((code << 3) >> 13).to_ulong()
			     + (code >> 13).to_ulong());
	};
	File get_dest_file() const {
	    return File(((code << 6) >> 13).to_ulong());
	};
	Rank get_dest_rank() const {
	    return Rank(((code << 9) >> 13).to_ulong());
	};
	Square get_dest() const {
	    return Square(16 * ((code << 9) >> 13).to_ulong()
			     + ((code << 6) >> 13).to_ulong());
	};
	MoveType get_type() const {
	    return MoveType(((code << 12) >> 12).to_ulong());
	};
	//bool is_capture() { return code[2]; }; // w/o null move
	bool is_capture() const {
	    return code[2] && (!is_null());
	}; 
	bool is_castle() const {
	    return !code[3] && !code[2] && code[1];
	};
	bool is_promotion() const {
	    return code[3];
	};
	bool is_double_pawn_push() const {
	    return get_type() == DOUBLE_PAWN_PUSH;
	};
	bool is_en_passant() const {
	    return get_type() == EN_PASSANT;
	};
	bool is_null() const {
	    return get_type() == NULL_MOVE;
	};
	PieceType get_promotion_type() const;
	PieceType get_castle_side() const;

	// Static member function for sorting move in natural order
	static bool numeric_comp(Move a, Move b) {
	    return (a.code.to_ulong() < b.code.to_ulong());
	};
	
	bool operator==(const Move& other) const {
	    return this->code == other.code;
	}
	bool operator!=(const Move& other) const {
	    return !(*this == other);
	}
};

#endif /* !MOVE_H */
