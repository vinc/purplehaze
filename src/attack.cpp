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
	Piece* ptr_piece;
	Square to;

	// Search for knights attacking s
	for (int i = 0; i < NB_KNIGHT_MOVES; ++i) {
		to = Square(s + KNIGHT_MOVES[i]);
		if (!b.is_off_the_board(to)) {
			ptr_piece = b.get_ptr_piece(to);
			if (ptr_piece && ptr_piece->get_type() == KNIGHT && ptr_piece->get_color() == c) {
				pieces.insert(*ptr_piece, BACK);
			}
		}			
	}
	
	// Search for others pieces attackings s
	for (int i = 0; i < NB_QUEEN_MOVES; ++i) {
		MoveOrientation offset = QUEEN_MOVES[i];
		to = s;
		bool capture_found = false;
		for (int dist = 1; dist < 8; ++dist) {
			to = Square(to + offset);
			if (b.is_off_the_board(to)) break;
			
			ptr_piece = b.get_ptr_piece(to);
			if (!ptr_piece) continue;
			if (ptr_piece->get_color() != c) break;
			assert(ptr_piece->get_position() == to);
			
			switch (ptr_piece->get_type()) {
				case PAWN:
					if (dist == 1 && 
							(
								(c == BLACK && (offset == UP_LEFT || offset == UP_RIGHT)) ||
								(c == WHITE && (offset == DOWN_LEFT || offset == DOWN_RIGHT))
							)
						) {
						capture_found = true;
					}
					break;
				case BISHOP:
					if (!b.have_same_file(s, to) && !b.have_same_rank(s, to)) {
						capture_found = true;
					}
					break;
				case ROOK:
					if (b.have_same_file(s, to) || b.have_same_rank(s, to)) {
						capture_found = true;
					}
					break;
				case KING:
					if ((dist != 1)/* || (b.get_ptr_piece(s) && b.get_ptr_piece(s)->get_type() == KING)*/) {
						break;
					}
				case QUEEN:
					capture_found = true;
					break;
				default:
					break;
			}
			if (capture_found) {
				pieces.insert(*ptr_piece, BACK);
			}
			break;
		}
	}
	return pieces;	
}

bool is_attacked(Board& b, Square s, Color c) {
	Piece* ptr_piece;
	Square to;

	// Search for knights attacking s
	for (int i = 0; i < NB_KNIGHT_MOVES; ++i) {
		to = Square(s + KNIGHT_MOVES[i]);
		if (!b.is_off_the_board(to)) {
			ptr_piece = b.get_ptr_piece(to);
			if (ptr_piece && ptr_piece->get_type() == KNIGHT && ptr_piece->get_color() == c) {
				return true;
			}
		}			
	}
	
	// Search for others pieces attackings s
	for (int i = 0; i < NB_QUEEN_MOVES; ++i) {
		MoveOrientation offset = QUEEN_MOVES[i];
		to = s;
		for (int dist = 1; dist < 8; ++dist) {
			to = Square(to + offset);
			if (b.is_off_the_board(to)) break;
			
			ptr_piece = b.get_ptr_piece(to);
			if (!ptr_piece) continue;
			if (ptr_piece->get_color() != c) break;
			assert(ptr_piece->get_position() == to);
			
			switch (ptr_piece->get_type()) {
				case PAWN:
					if (dist == 1 && 
							(
								(c == BLACK && (offset == UP_LEFT || offset == UP_RIGHT)) ||
								(c == WHITE && (offset == DOWN_LEFT || offset == DOWN_RIGHT))
							)
						) {
						return true;
					}
					break;
				case BISHOP:
					if (!b.have_same_file(s, to) && !b.have_same_rank(s, to)) {
						return true;
					}
					break;
				case ROOK:
					if (b.have_same_file(s, to) || b.have_same_rank(s, to)) {
						return true;
					}
					break;
				case KING:
					if ((dist != 1)/* || (b.get_ptr_piece(s) && b.get_ptr_piece(s)->get_type() == KING)*/) {
						break;
					}
				case QUEEN:
					return true;
					break;
				default:
					break;
			}
			break;
		}
	}
	return false;	
}
