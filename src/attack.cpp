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

Pieces is_attacked_by(Board& b, Square s, Color c) {
	Pieces pieces;
	Piece* ptr_piece = 0;
	Square to;

	// Search for knights attacking s
	for (int i = 0; i < NB_KNIGHT_MOVES; ++i) {
		to = Square(s + KNIGHT_MOVES[i]);
		if (!b.is_off_the_board(to)) {
			ptr_piece = b.get_ptr_piece(to);
			if (ptr_piece && ptr_piece->get_type() == KNIGHT && ptr_piece->get_color() == c) {
				//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
				pieces.insert(*ptr_piece, BACK);
			}
		}			
	}
	
	// Search for others pieces attackings s
	for (int i = 0; i < NB_QUEEN_MOVES; ++i) {
		MoveOrientation offset = QUEEN_MOVES[i];
		to = Square(s + offset);
		if (!b.is_off_the_board(to)) {
			ptr_piece = b.get_ptr_piece(to);
			
			if (ptr_piece && ptr_piece->get_color() == c) {
				// Pawn's attack
				if (ptr_piece->get_type() == PAWN && (
					(c == BLACK && (offset == UP_LEFT || offset == UP_RIGHT)) ||
					(c == WHITE && (offset == DOWN_LEFT || offset == DOWN_RIGHT))
					)) {
					//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
					pieces.insert(*ptr_piece, BACK);
				}
				
				// Rook's attack
				else if (ptr_piece->get_type() == ROOK && (
					b.get_rank(s) == b.get_rank(to) || b.get_file(s) == b.get_file(to)
					)) {
					//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
					pieces.insert(*ptr_piece, BACK);
				}
				
				// Bishop's attack
				else if (ptr_piece->get_type() == BISHOP && (
						b.get_rank(s) != b.get_rank(to) && b.get_file(s) != b.get_file(to)
						)) {
					//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
					pieces.insert(*ptr_piece, BACK);
				}
				
				// Queen's or king's attack
				else if (ptr_piece->get_type() == QUEEN || ptr_piece->get_type() == KING) {
					//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
					pieces.insert(*ptr_piece, BACK);
				}
			}
			else {
				while (!ptr_piece && !b.is_off_the_board(Square(to + offset))) {
					to = Square(to + offset);
					ptr_piece = b.get_ptr_piece(to);
				}
				ptr_piece = b.get_ptr_piece(to);
				if (ptr_piece && ptr_piece->get_color() == c) {

					//TODO Weird stuff here, piece's position should be Square to, but sometime it's not.
					if (ptr_piece->get_position() != to) {
						cout << *ptr_piece << " " << hex << ptr_piece->get_position() << " " << hex << to << endl;
					}

					
					// Rook's attack
					if (ptr_piece->get_type() == ROOK && (
						b.get_rank(s) == b.get_rank(to) || b.get_file(s) == b.get_file(to)
						)) {
						//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
						pieces.insert(*ptr_piece, BACK);
					}
				
					// Bishop's attack
					else if (ptr_piece->get_type() == BISHOP && (
						b.get_rank(s) != b.get_rank(to) && b.get_file(s) != b.get_file(to)
						)) {
						//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
						pieces.insert(*ptr_piece, BACK);
					}
				
					// Queen's attack
					else if (ptr_piece->get_type() == QUEEN) {
						//cout << *ptr_piece << " in " << hex << to << " is attacking " << hex << s << "! ";
						pieces.insert(*ptr_piece, BACK);
					}
				}
			}
		}
	}
	return pieces;	
}
