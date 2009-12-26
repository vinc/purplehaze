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

/*
 * Move generator for Knight and King
 */

#include <string> 
#include <iomanip>
#include <iostream>

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"
#include "init.h"

/*
 * Initialise the game according to a FEN record.
 *
 * For example the starting position in chess is :
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
 *
 */

void init_board(const char* f) {
	string fen(f);
	string::const_iterator it;

	Square s = A8;
	for (it = fen.begin(); it != fen.end(); ++it) {
		//cout << *it;
		
		// Space separator
		if (*it == ' ') {
			break;
		}
		// Slash separator
		else if (*it == '/') {
			s = Square(s - 0x18);
		}
		
		// Empty squares
		else if (int(*it) >= ASCII_1 && int(*it) <= ASCII_8) {
			s = Square(s + *it - ASCII_1 + 1);
		}
		
		// Non empty square
		else {
			Color c = UNDEF_COLOR;
			PieceType pt = UNDEF_PIECE_TYPE;
			Position p = UNDEF_POSITION;
			switch (*it) {
				case 'p': c = BLACK, pt = PAWN; p = BACK; break;
				case 'n': c = BLACK, pt = KNIGHT; p = BACK; break;
				case 'b': c = BLACK, pt = BISHOP; p = BACK; break;
				case 'r': c = BLACK, pt = ROOK; p = BACK; break;
				case 'q': c = BLACK, pt = QUEEN; p = BACK; break;
				case 'k': c = BLACK, pt = KING; p = FRONT; break;
				case 'P': c = WHITE, pt = PAWN; p = BACK; break;
				case 'N': c = WHITE, pt = KNIGHT; p = BACK; break;
				case 'B': c = WHITE, pt = BISHOP; p = BACK; break;
				case 'R': c = WHITE, pt = ROOK; p = BACK; break;
				case 'Q': c = WHITE, pt = QUEEN; p = BACK; break;
				case 'K': c = WHITE, pt = KING; p = FRONT; break;
				default: break;
		
			}
			switch (c) {
				case BLACK:	black_pieces.insert(*new Piece(BLACK, pt, s), p); break;
				case WHITE: white_pieces.insert(*new Piece(WHITE, pt, s), p); break;
				default: break;
			}
			board.zobrist.set_piece(c, pt, s);
			//cout << board.zobrist << endl;
			s = Square(s + 1);
		}
	}

	for (white_pieces.iterator = white_pieces.begin(); white_pieces.iterator != white_pieces.end(); ++white_pieces.iterator) {
		Piece* ptr_piece = white_pieces.get_ptr_piece();
		board.set_ptr_piece(ptr_piece, ptr_piece->get_position());
	}
	board.have_castled(WHITE, false);
	
	//board.print();
	
	for (black_pieces.iterator = black_pieces.begin(); black_pieces.iterator != black_pieces.end(); ++black_pieces.iterator) {
		Piece* ptr_piece = black_pieces.get_ptr_piece();
		board.set_ptr_piece(ptr_piece, ptr_piece->get_position());
	}
	board.have_castled(BLACK, false);
	
	//board.print();
}
