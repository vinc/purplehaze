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

#ifndef NODE_H
#define NODE_H

#include <bitset>

#include "common.h"
#include "pieces.h"
#include "zobrist.h"

using namespace std;

class Node
{
    private:
	Color side_to_move;
	short halfmove_counter;
	short score;
	short ply;
	bitset<4> castle_rights;
	Square en_passant;
	
	Piece capture;
	Hash zobrist_hash;

    public:
	Node() : 
	    side_to_move(WHITE), 
	    halfmove_counter(0),
	    score(0),
	    ply(0),
	    //castle_rights(0), // Assume default
	    en_passant(OUT) {}
	
	Hash& hash() { return zobrist_hash; };
	Color get_turn_color() const {
	    return side_to_move;
	};
	void change_side() {
	    side_to_move = Color(!side_to_move);
	};
	short get_ply() const {
	    return ply;
	};
	void inc_ply() {
	    --ply;
	};
	void dec_ply() {
	    --ply;
	};
	short get_halfmove() const {
	    return halfmove_counter;
	};
	void set_halfmove(short i) {
	    halfmove_counter = i;
	};
	void inc_halfmove() {
	    ++halfmove_counter;
	};
	void reset_halfmove() {
	    halfmove_counter = 0;
	};
	Square get_en_passant() const {
	    return en_passant;
	};
	void set_en_passant(Square ep) {
	    en_passant = ep;
	};
	Piece get_capture() const {
	    return capture;
	};
	void set_capture(Piece p) {
	    capture = p;
	};
	bool can_castle(Color c, PieceType t) const {
	    return castle_rights[2 * c + t - QUEEN];
	};
	void set_castle_right(Color c, PieceType t, bool b = true) {
	    castle_rights.set(2 * c + t - QUEEN, b);
	};
};

#endif /* !NODE_H */
