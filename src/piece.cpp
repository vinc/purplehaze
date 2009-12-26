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

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"

using namespace std;

ostream& operator<<(ostream& out, const Piece& piece) {
	switch (piece.get_type()) {
		case PAWN: out << 'P'; break;
		case KNIGHT: out << 'N'; break;
		case BISHOP: out << 'B'; break;
		case ROOK: out << 'R'; break;
		case QUEEN: out << 'Q'; break;
		case KING: out << 'K'; break;
		default: out << "#UNDEF"; break;
	}
    return out;
}

Piece::Piece() {
	position = OUT;
	color = UNDEF_COLOR;
	type = UNDEF_PIECE_TYPE;
	nb_moves = 0;
}

Piece::Piece(Color c, PieceType t) : color(c), type(t) {
	position = OUT;
	nb_moves = 0;
}

Piece::Piece(Color c, PieceType t, Square s) : color(c), type(t), position(s) {
	nb_moves = 0;
}

/*
bool Piece::operator<(const Piece &piece) const {
	return 200 * this->type + this->position < 200 * piece.type + piece.position;
}
*/

bool Piece::operator<(const Piece &piece) const {
	return this->get_value() > piece.get_value();
}

Color Piece::get_color() const {
	return color;
}

PieceType Piece::get_type() const {
	return type;
}

void Piece::set_type(PieceType t) {
	type = t;
}

PieceValue Piece::get_value() const {
	switch (type) {
		case PAWN: return PAWN_VALUE;
		case KNIGHT: return KNIGHT_VALUE;
		case BISHOP: return BISHOP_VALUE;
		case ROOK: return ROOK_VALUE;
		case QUEEN: return QUEEN_VALUE;
		case KING: return KING_VALUE;
		case UNDEF_PIECE_TYPE: return UNDEF_PIECE_VALUE;
	}
	return UNDEF_PIECE_VALUE;
}

Square Piece::get_position() const {
	return position;
}

void Piece::set_position(Square s) {
	position = s;
}

int Piece::get_nb_moves() const {
	return nb_moves;
}
void Piece::inc_nb_moves() {
	++nb_moves;
}
void Piece::dec_nb_moves() {
	--nb_moves;
}

char* Piece::to_string(char* ptr_buf) {
	char str[4];
	if (color == BLACK)
		str[0] = '-';
	else
		str[0] = ' ';
	
	switch (type) {
		case PAWN: str[1] = 'P'; break;
		case KNIGHT: str[1] = 'N'; break;
		case BISHOP: str[1] = 'B'; break;
		case ROOK: str[1] = 'R'; break;
		case QUEEN: str[1] = 'Q'; break;
		case KING: str[1] = 'K'; break;
		default: str[1] = ' '; break;
	}
	if (color == BLACK)
		str[2] = '-';
	else
		str[2] = ' ';
	str[3] = '\0';
	strcpy(ptr_buf, str);
	return ptr_buf;
}

Pieces::Pieces() {
	color = UNDEF_COLOR;
}

Pieces::Pieces(Color c) : color(c) {}

Color Pieces::get_color() const {
	return color;
}

void Pieces::insert(Piece& piece, Position position) {
	switch (position) {
		case FRONT: pieces.push_front(piece); break;
		case BACK: pieces.push_back(piece); break;
		default: pieces.push_back(piece); break;
	}
}


Piece* Pieces::get_ptr_piece() {
	Piece* ptr_piece = &*iterator;
	return ptr_piece;
}

Piece* Pieces::get_ptr_king() {
	Piece* ptr_king = &pieces.front();
	return ptr_king;
}

Piece* Pieces::get_ptr_smallest() {
	Piece* ptr_smallest = &pieces.back();
	return ptr_smallest;
}
list<Piece>::iterator Pieces::begin() {
	return pieces.begin();
}

list<Piece>::iterator Pieces::end() {
	return pieces.end();
}

int Pieces::size() {
	return pieces.size();
}

void Pieces::clear() {
	pieces.clear();
}

void Pieces::sort() {
	pieces.sort();
}
