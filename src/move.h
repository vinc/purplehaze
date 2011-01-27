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
#include <string>

#include "common.h"

using namespace std;

class Move
{
    friend ostream& operator<<(ostream& out, const Move move);
    
    protected:
	/*
	 * A move is coded using 16 bits:
	 *     4 bits for the type
	 *     3 bits for the destination square rank
	 *     3 bits for the destination square file 
	 *     3 bits for the origin square rank
	 *     3 bits for the origin square file 
	 */
	
	static const int MT_MASK = 0xF;
	static const int DR_MASK = 0x7;
	static const int DF_MASK = 0x7;
	static const int OR_MASK = 0x7;
	static const int OF_MASK = 0x7;
	
	static const int MT_SHIFT = 0;
	static const int DR_SHIFT = 4;
	static const int DF_SHIFT = 7;
	static const int OR_SHIFT = 10;
	static const int OF_SHIFT = 13;
	
	//bitset<16> code;
	short code;
	//Move(bitset<16> c) : code(c) {}
	
	bool is_set(int i) const { return code & ( 1 << i); };
    
    public:
	Move() : code(NULL_MOVE) {}
	Move(Square o, Square d, MoveType t){
	    code = ((o & 7) << OF_SHIFT) | 
		   ((o >> 4) << OR_SHIFT) |
		   ((d & 7) << DF_SHIFT) | 
		   ((d >> 4) << DR_SHIFT) | 
		   (t);
	};
	
	friend class ExtendedMove;
	
	File get_orig_file() const {
	    //return File((code >> 13).to_ulong());
	    return File((code >> OF_SHIFT) & OF_MASK);
	};
	Rank get_orig_rank() const {
	    //return Rank(((code << 3) >> 13).to_ulong());
	    return Rank((code >> OR_SHIFT) & OR_MASK);
	};
	File get_dest_file() const {
	    return File((code >> DF_SHIFT) & DF_MASK);
	};
	Rank get_dest_rank() const {
	    return Rank((code >> DR_SHIFT) & DR_MASK);
	};
	Square get_orig() const {
	    //return Square(16 * ((code << 3) >> 13).to_ulong()
	    //		     + (code >> 13).to_ulong());
	    return Square(16 * get_orig_rank() + get_orig_file());
	};
	Square get_dest() const {
	    //return Square(16 * ((code << 9) >> 13).to_ulong()
	    //		     + ((code << 6) >> 13).to_ulong());
	    return Square(16 * get_dest_rank() + get_dest_file());
	};
	MoveType get_type() const {
	    //return MoveType(((code << 12) >> 12).to_ulong());
	    return MoveType((code >> MT_SHIFT) & MT_MASK);
	};


	//bool is_capture() { return code[2]; }; // w/o null move
	bool is_capture() const {
	    //return code[2] && (!is_null());
	    return is_set(2) && (!is_null());
	}; 
	bool is_castle() const {
	    //return !code[3] && !code[2] && code[1];
	    return !is_set(3) && !is_set(2) && is_set(1);
	};
	bool is_promotion() const {
	    //return code[3];
	    return is_set(3);
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
	    //return (a.code.to_ulong() < b.code.to_ulong());
	    return (a.code < b.code);
	};
	
	bool operator==(const Move& other) const {
	    return this->code == other.code;
	}
	bool operator!=(const Move& other) const {
	    return !(*this == other);
	}
	string to_string() const;
};

class ExtendedMove : public Move
{
    private:
	//bitset<16> code;
	//bitset<16> extra;
	//ExtendedMove(bitset<16> c, bitset<16> e) : code(c), extra(e) {}
	char score;
    public:
	ExtendedMove() : score(0) { code = Move().code; }
	ExtendedMove(Move m, int s = 0) : score(s) { code = m.code; }
	void set_score(int s) { score = s; };
	bool operator<(const ExtendedMove& other) const {
	    return this->score > other.score; // FIXME replace '<' by '>'
	}

};

#endif /* !MOVE_H */
