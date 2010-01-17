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
#include <iomanip>
#include <iostream>

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"
#include "init.h"

#ifdef DEBUG
bool fen_debug = true;
#else
bool fen_debug = false;
#endif

/** Initialise the game according to a FEN record.
  * For example the starting position in chess is :
  * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
  */
void init_board(const char* f) {
	string fen(f);
	assert(fen.length() > 0);
	string::const_iterator it;
	
	//cout << "Cleaning the board..." << endl;
	board = Board();
	white_pieces.clear();
	black_pieces.clear();
	//cout << "The board is clean!" << endl;
	
	if (fen_debug) cout << "FEN: parsing: " << fen << endl;
	// Parse the FEN for discovering pieces
	Square s = A8;
	for (it = fen.begin(); it != fen.end(); ++it) {
		// Space separator
		if (*it == ' ') {
			// We have found all the pieces we have to found
			break;
		}
		// Slash separator
		else if (*it == '/') {
			// This is the begining of a new rank
			s = Square(s - 0x18);
		}
		
		// Empty squares
		else if (int(*it) >= ASCII_1 && int(*it) <= ASCII_8) {
			// Find the next square
			s = Square(s + *it - ASCII_1 + 1);
		}
		
		// Non empty square
		else {
			// define a new piece
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
			
			// Put the piece with black or white pieces
			Piece piece(c, pt, s);
			switch (c) {
				//case BLACK:	black_pieces.insert(*new Piece(BLACK, pt, s), p); break;
				//case WHITE: white_pieces.insert(*new Piece(WHITE, pt, s), p); break;
				case BLACK:	black_pieces.insert(piece, p); break;
				case WHITE: white_pieces.insert(piece, p); break;
				default: break;
			}
			
			// Update the board's Zobrist key
			board.zobrist.set_piece(c, pt, s);

			// Find the next square
			s = Square(s + 1);
		}
	}
	++it;

	// Set up the white pieces on the board
	for (white_pieces.iterator = white_pieces.begin(); white_pieces.iterator != white_pieces.end(); ++white_pieces.iterator) {
		Piece* ptr_piece = white_pieces.get_ptr_piece();
		board.set_ptr_piece(ptr_piece, ptr_piece->get_position());
	}

	// Set up the black pieces on the board
	for (black_pieces.iterator = black_pieces.begin(); black_pieces.iterator != black_pieces.end(); ++black_pieces.iterator) {
		Piece* ptr_piece = black_pieces.get_ptr_piece();
		board.set_ptr_piece(ptr_piece, ptr_piece->get_position());
	}
	
	// Set the side to move
	switch(*it) {
		case 'w':
			board.set_turn_color(WHITE);
			if (fen_debug) cout << "FEN: white to play" << endl;
			break;
		case 'b':
			board.set_turn_color(BLACK);
			if (fen_debug) cout << "FEN: black to play" << endl;
			break;
		default:
			cerr << "Error while parsing FEN: could not found the site to move!" << endl;
			break;
	}
	++it; // Skip the space char
	++it;

	// Parse the FEN for discovering castling abilities
	for (; it != fen.end(); ++it) {
		bool white_can_castle = false;
		bool black_can_castle = false;
		//cout << "FEN: parse castle: " << *it << endl;
		if (*it == ' ') {
				break; // Exit loop
		}
		switch(*it) {
			case '-': // No castling abilities
				break;
			case 'K':
			case 'Q':
				white_can_castle = true;
				break;
			case 'k':
			case 'q':
				black_can_castle = true;
				break;
		}
		if (white_can_castle) {
			board.set_castling_right(WHITE, true);			
			if (fen_debug) cout << "FEN: white can castle" << endl;
		}
		else if (white_pieces.get_ptr_king()->get_nb_moves() == 0) {
			// Fix a bug when the king have castle but still have his move counter at 0
			white_pieces.get_ptr_king()->inc_nb_moves();
		}
		if (black_can_castle) {
			board.set_castling_right(BLACK, true);
			if (fen_debug) cout << "FEN: black can castle" << endl;
		}
		else if (black_pieces.get_ptr_king()->get_nb_moves() == 0) {
			// Fix a bug when the king have castle but still have his move counter at 0
			black_pieces.get_ptr_king()->inc_nb_moves();
		}
	}
	++it;
	
	// Set the en passant square if any
	if (*it != '-') {
		char file = *it;
		++it;
		char rank = *it;
		Square s = Square((rank - '1') * 16 + file - 'a');
		board.set_en_passant(s);
		if (fen_debug) cout << "FEN: en passant square fixed to " << static_cast<char>(97 + (s & 7)) << 1 + (s >> 4) << endl;
	}
	++it; // Skip the space char
	++it;

	// Parse the FEN for setting the 50 moves counter
	int half_move = 0;
	for (; it != fen.end(); ++it) {
		// Space separator
		if (*it == ' ') {
			board.set_repetitions(half_move);
			if (fen_debug) cout << "FEN: board repetition fixed to " << half_move << endl;
			break;
		}
		else {
			half_move = half_move * 10 + *it - '0';
		}
	}
	++it;	
}
