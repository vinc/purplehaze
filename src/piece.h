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

#include <iostream>
#include <string> 
#include <cstdlib>
#include <cstring>
#include <list>

#include "common.h"

class Piece
{
	friend ostream& operator<<(ostream& out, const Piece& piece);
	
	private:
		unsigned int nb_moves;
		Color color;
		PieceType type;
		Square position;
		
	public:
		Piece();
		Piece(Color c, PieceType t);
		Piece(Color c, PieceType t, Square s);
		bool operator<(const Piece &piece) const;
		Color get_color() const {return color;};
		PieceType get_type() const {return type;};
		void set_type(PieceType t);
		PieceValue get_value() const;
		Square get_position() const {return position;};
		void set_position(Square s);
		unsigned int get_nb_moves() const {return nb_moves;};
		void inc_nb_moves() {++nb_moves;};
		void dec_nb_moves() {--nb_moves;};
		char* to_string(char* ptr_buf);
};

class Pieces
{
	private:
		list<Piece> pieces;
		Color color;
	public:
		Pieces();
		Pieces(Color c);
		Color get_color() const {return color;};
		void insert(Piece piece, Position position);
		Piece* get_ptr_piece();
		Piece* get_ptr_king();
		Piece* get_ptr_smallest();
		//typedef std::set<Piece*>::iterator iterator;
		list<Piece>::iterator iterator;
		list<Piece>::iterator begin() {return pieces.begin();};
		list<Piece>::iterator end() {return pieces.end();};
		int size() {return pieces.size();};
		void clear() {return pieces.clear();};
		void sort() {return pieces.sort();};
};
