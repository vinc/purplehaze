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

ostream& operator<<(ostream& out, const Zobrist& zobrist) {
	out << zobrist.get_key();
    return out;
}


// Generate a random Hash
Hash Zobrist::gen_hash() {
	//const int rand_max_bit = 15; // Assumes RAND_MAX is 32767

	//Hash h = rand() ^ ((Hash)rand() << 15) ^ ((Hash)rand() << 30) ^ ((Hash)rand() << 45) ^ ((Hash)rand() << 60);
	//Hash h = generator() ^ ((Hash) generator() << 15) ^ ((Hash) generator() << 30) ^ ((Hash) generator() << 45) ^ ((Hash) generator() << 60);
	//Hash h = generator();
	Hash h = generator() ^ ((Hash) generator() << 32);
	//Hash h = rand();
	//cout << hex << h << dec << endl;
	return h;
}

Zobrist::Zobrist() {
	generator.seed(5489u);

	for (int c = 0; c < 2; ++c) {
		for (int pt = 0; pt < 6; ++pt) {
			for (int s = 0; s < BOARD_SIZE; ++s) {
				piece_square[c][pt][s] = gen_hash();
			}
		}
	}
	side_to_move_is_black = gen_hash();
	for (int cr = 0; cr < 4; ++cr) {
		castling_rights[cr] = gen_hash();
	}
	for (int ep = 0; ep < BOARD_SIZE; ++ep) {
		en_passant_square[ep] = gen_hash();
	}
	zobrist_key = 0;
}
/*
void Zobrist::set_piece(PieceType pt, Color c, Square s) {
	zobrist_key ^= piece_square[p.get_type()][p.get_color()][s];
}
*/
void Zobrist::set_piece(Color c, PieceType pt, Square s) {
	//cout << "c= " << c << " pt=" << pt << " s=" << s << endl;
	zobrist_key ^= piece_square[c][pt][s];
}

void Zobrist::add_move(Move& m) {
	// Change side
	zobrist_key ^= side_to_move_is_black;

	// If it is a null move we are done here
	if (m.get_type() == UNDEF_MOVE_TYPE) return;

	Color c = m.get_ptr_piece()->get_color();
	Square to = m.get_to();
	PieceType pt = m.get_ptr_piece()->get_type();
		
	// Remove the piece from his previous position
	zobrist_key ^= piece_square[c][pt][m.get_from()];

	// Special treatement
	switch (m.get_type()) {
		case MOVE:
			break;
		case EN_PASSANT:
			// Remove the captured pawn from his position
			Square pawn_position;
			pawn_position = (c == WHITE) ? Square(to + DOWN) : Square(to + UP);
			zobrist_key ^= piece_square[m.get_ptr_captured_piece()->get_color()][m.get_ptr_captured_piece()->get_type()][pawn_position];	
			break;	
		case CAPTURE:
			// Remove the captured piece from his position
			zobrist_key ^= piece_square[m.get_ptr_captured_piece()->get_color()][m.get_ptr_captured_piece()->get_type()][to];	
			break;
		case CASTLE:
			Square rook_from, rook_to;
			switch (to) {
				case G1: rook_from = H1; rook_to = F1; break;
				case C1: rook_from = A1; rook_to = D1; break;
				case G8: rook_from = H8; rook_to = F8; break;
				case C8: rook_from = A8; rook_to = D8; break;
				default: rook_from = OUT; rook_to = OUT; break;			
			}
			// Remove the rook from his previous position
			zobrist_key ^= piece_square[c][ROOK][rook_from];

			// Place the rook to his new position
			zobrist_key ^= piece_square[c][ROOK][rook_to];		
			break;
		default:
			break;
	}
	// Promotion
	if (m.get_promotion() != UNDEF_PIECE_TYPE) {
		// Remove the pawn
		zobrist_key ^= piece_square[c][PAWN][to];
		
		// Place the new piece
		zobrist_key ^= piece_square[c][m.get_promotion()][to];
	}
	
	// Place the piece to his new position
	zobrist_key ^= piece_square[c][pt][to];

}
void Zobrist::sub_move(Move& m) {
	// By doing the same things as make we can unmake the move
	this->add_move(m);
}

Hash Zobrist::get_key() const {
	return zobrist_key;
}
void Zobrist::set_key(Hash h) {
	zobrist_key = h;
}
