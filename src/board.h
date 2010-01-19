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

		#ifdef KILLER_HEURISTIC
		Move* killer_moves[2][MAX_DEPTH];
		#endif
	public:
		Board();
		
		Zobrist zobrist;
		list<Hash> positions_history;
		int ply;
		
		Piece* get_ptr_piece(Square s) const;
		void set_ptr_piece(Piece* ptr_piece, Square s);		
		Pieces get_pieces() const;
		Square get_square_above(Square s) const {return Square(s + 0x10);};		
		Square get_square_right(Square s) const {return Square(s + 0x1);};		
		Square get_square_below(Square s) const {return Square(s - 0x10);};		
		Square get_square_left(Square s) const {return Square(s - 0x1);};		
		int get_file(Square s) const {return (s & 7);};
		int get_rank(Square s) const {return (s >> 4);};
		int have_same_file(Square a, Square b) const {return (a & 7) == (b & 7);};
		int have_same_rank(Square a, Square b) const {return (a >> 4) == (b >> 4);};
		Color get_turn_color() const {return turn_color;};		
		void set_turn_color(Color c) {turn_color = c;};
		void change_turn_color() {turn_color = (turn_color == WHITE ? BLACK : WHITE);};
		bool is_off_the_board(Square s) const {return s & 0x88;}
		bool is_dark(Square s) const;
		void print() const;
		static string square_to_string(Square s);
		
		void set_castling_right(Color c, bool b); //TODO change to set_castle(...)
		bool have_castled(Color c); //TODO change to have_castled(...)
		
		Square get_en_passant() const {return en_passant;};
		void set_en_passant(Square ep);
		
		void set_repetitions(int r);
		int get_repetitions() const {return nb_repetitions;};
		void inc_repetitions() {++nb_repetitions;};
		void dec_repetitions() {--nb_repetitions;};
		void reset_repetitions() {nb_repetitions = 0;};
		
		/*
		int get_distance(Square a, Square b) const;
		MoveOrientation get_direction(Square a, Square b) const;
		*/

		#ifdef KILLER_HEURISTIC
		void put_killer_move(Move* ptr_move, int depth);
		bool is_first_killer_move(Move* ptr_move, int depth) const;
		bool is_second_killer_move(Move* ptr_move, int depth) const;
		#endif
};

#endif /* !BOARD_H */
