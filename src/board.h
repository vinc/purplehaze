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

#include <string>

#include "common.h"
#include "zobrist.h"

#ifndef BOARD_H
#define BOARD_H

class Board
{
	private:
		Piece* board[BOARD_SIZE];
		Color turn_color;
		bool black_have_castled;
		bool white_have_castled;
		Square en_passant;
		int nb_repetitions;
	public:
		Board();
		Zobrist zobrist;
		list<Hash> positions_history;
		Piece* get_ptr_piece(Square s) const;
		void set_ptr_piece(Piece* ptr_piece, Square s);		
		Pieces get_pieces() const;
		Square get_square_above(Square s) const;		
		Square get_square_right(Square s) const;		
		Square get_square_below(Square s) const;		
		Square get_square_left(Square s) const;		
		int get_file(Square s) const;
		int get_rank(Square s) const;
		Color get_turn_color() const;		
		void set_turn_color(Color c);
		void change_turn_color();
		bool is_off_the_board(Square s) const {return s & 0x88;}
		bool is_dark(Square s) const;
		void print() const;
		static string square_to_string(Square s);
		void set_castling_right(Color c, bool b); //TODO change to set_castle(...)
		bool have_castled(Color c); //TODO change to have_castled(...)
		Square get_en_passant() const;
		void set_en_passant(Square ep);
		void inc_repetitions();
		void dec_repetitions();
		void reset_repetitions();
		void set_repetitions(int r);
		int get_repetitions() const;
};

#endif /* !BOARD_H */
