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
#include "move.h"
#include "zobrist.h"

using namespace std;

class Position
{
    private:
	Hash zobrist_hash;
	Hash material_zobrist_hash;
	short ply;
	//Move last_move;
	Piece capture;
	bitset<4> castle_rights;
	bitset<2> castle;
	Square en_passant;
	unsigned char halfmove_counter;
	bool null_move_right;
	Color side_to_move;

    public:
	Position() : 
	    ply(0),
	    en_passant(OUT),
	    halfmove_counter(0),
	    null_move_right(true),
	    side_to_move(WHITE) 
	    {} 
	
	Hash& hash() { 
	    return zobrist_hash;
	};
	Hash& material_hash() { 
	    return material_zobrist_hash; 
	};
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
	    ++ply;
	};
	void dec_ply() {
	    --ply;
	};
	unsigned char get_halfmove() const {
	    return halfmove_counter;
	};
	void set_halfmove(unsigned char i) {
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
	bool has_castle(Color c) const {
	    return castle[c];
	};
	void set_has_castle(Color c, bool b = true) {
	    castle.set(c, b);
	};
	bool get_null_move_right() const {
	    return null_move_right;
	};
	void set_null_move_right(bool b) {
	    null_move_right = b;
	};
};

#endif /* !NODE_H */
