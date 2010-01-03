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
#include <cstdlib>
#include <string>

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"

#include "eval.h"
#include <iomanip>

using namespace std;

Board::Board()	{
	turn_color = WHITE;
	en_passant = OUT;
	nb_repetitions = 0;
	black_have_castled = false;
	white_have_castled = false;
	int i;
	for (i = 0; i < BOARD_SIZE; ++i)
		board[i] = 0;

}

Piece* Board::get_ptr_piece(Square s) const {
	if (!is_off_the_board(s) && board[s])
		return board[s];
	else
		return 0;
}

void Board::set_ptr_piece(Piece* ptr_piece, Square s) {
	if (ptr_piece) {
		board[ptr_piece->get_position()] = 0;
		board[s] = ptr_piece;
		ptr_piece->set_position(s);
	}
}

Pieces Board::get_pieces() const {
	Pieces pieces;
	int i;
	for (i = 0; i < BOARD_SIZE; ++i)
		if (board[i])
			pieces.insert(*board[i], BACK);
	return pieces;
}

Square Board::get_square_above(Square s) const {
	return Square(s + 0x10);
}

Square Board::get_square_right(Square s) const {
	return Square(s + 0x1);
}

Square Board::get_square_below(Square s) const {
	return Square(s - 0x10);
}

Square Board::get_square_left(Square s) const {
	return Square(s - 0x1);
}

int Board::get_file(Square s) const {
	return (s & 7);
}

int Board::get_rank(Square s) const {
	return (s >> 4);
}

Color Board::get_turn_color() const {
	return turn_color;
}

void Board::set_turn_color(Color c) {
	turn_color = c;
}

void Board::change_turn_color() {
	turn_color = (turn_color == WHITE ? BLACK : WHITE);
}	

/*
// Inlined
bool Board::is_off_the_board(Square s) const {
	return s & 0x88;
}
*/

bool Board::is_dark(Square s) const {
	return (s & 7) % 2 == (s >> 4) % 2;
}

string Board::square_to_string(Square s) {
	string res = "";
	res += char(97 + (s & 7));
	res += char(49 + (s >> 4));
	return res;
}

void Board::print() const {
	char ptr_buf[4];
	Piece* ptr_p;
	Square s;
	cout << endl;
	for (s = A8; s >= A1; s = Square(s+1)) {
		if (!is_off_the_board(s)) {
			// Begining of a rank
			if (get_file(s) == 0)
				cout << "     +---+---+---+---+---+---+---+---+" << endl << "   " << get_rank(s) + 1 << " ";
			
			// Begining of a square
			cout << "|";
			
			
			// Square
			ptr_p = get_ptr_piece(s);
			if (ptr_p)
				cout << ptr_p->to_string(ptr_buf);
			else if (is_dark(s))
				cout << " . ";
			else
				cout << "   ";
			
			
			//cout << setw(3) << PAWN_PCSQ[s];
			//cout << setw(3) << PAWN_PCSQ[FLIP[s]];
			
			// Ending of a rank
			if (get_file(s) == 7) {
				cout << "|" << endl;
				// End of board?
				if (s == H1)
					break;
				else
					s = Square(s - 0x18);
			}	
		}
	}
	cout << "     +---+---+---+---+---+---+---+---+" << endl;
	cout << "       a   b   c   d   e   f   g   h  " << endl << endl;
}

void Board::set_castling_right(Color c, bool b) {
	(c == WHITE) ? white_have_castled = !b : black_have_castled = !b;
}
bool Board::have_castled(Color c) {
	return (c == WHITE) ? white_have_castled : black_have_castled;
}


Square Board::get_en_passant() const {
	return en_passant;
}

void Board::set_en_passant(Square ep) {
	en_passant = ep;
}

void Board::inc_repetitions() {
	++nb_repetitions;
}

void Board::dec_repetitions() {
	--nb_repetitions;
}

void Board::reset_repetitions() {
	nb_repetitions = 0;
}

void Board::set_repetitions(int r) {
	nb_repetitions = r;
}

int Board::get_repetitions() const {
	return nb_repetitions;
}