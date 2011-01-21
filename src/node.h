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

using namespace std;

class Node
{
    private:
	    Color side_to_move;
	    short half_move_counter;
	    short score;
	    short ply;
	    bitset<4> castle_rights;
	    Square en_passant;
	    Piece capture; // TODO: do we realy need this?
    public:
	    Node();
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
	        ply++;
	    };
	    void dec_ply() {
	        ply--;
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
	    bool can_castle(Color c, PieceType t) const;
	    void set_castle_right(Color c, PieceType t, bool b = true);
};

#endif /* !NODE_H */
