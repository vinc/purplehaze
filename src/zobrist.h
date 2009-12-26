/*  PurpleHaze 1.0
    Copyright (C) 2007-2009  Vincent Ollivier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.h"
#include "move.h"

#ifndef ZOBRIST_H
#define ZOBRIST_H

class Zobrist
{
	friend ostream& operator<<(ostream& out, const Zobrist& zobrist);
	
	private:
		Hash gen_hash();
		Hash zobrist_key;
		Hash piece_square[2][6][BOARD_SIZE];
		Hash side_to_move_is_black;
		Hash castling_rights[4];
		Hash en_passant_square[BOARD_SIZE];
	public:
		Zobrist();
		void set_piece(Color c, PieceType pt, Square s);
		void add_move(Move& m);
		void sub_move(Move& m);
		Hash get_key() const;
		void set_key(Hash h);
};

#endif /* !ZOBRIST_H */
