/* PurpleHaze 2.0.0
 * Copyright (C) 2007-2011  Vincent Ollivier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <iostream>
#include <list>
#include <vector>

#include "game.h"

using namespace std;

typedef list<Direction> Directions;

const Directions NO_DIRS = { 
    NO_DIR
};
const Directions KNIGHT_DIRS = {
    UP_UP_RIGHT, RIGHT_UP_RIGHT, RIGHT_DOWN_RIGHT, DOWN_DOWN_RIGHT, 
    DOWN_DOWN_LEFT, LEFT_DOWN_LEFT, LEFT_UP_LEFT, UP_UP_LEFT
};
const Directions BISHOP_DIRS = {
    UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT
};
const Directions ROOK_DIRS = {
    UP, RIGHT, DOWN, LEFT
};
const Directions QUEEN_DIRS = {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
};
const Directions KING_DIRS = {
    UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
};

const vector<Directions> PIECES_DIRS = {
    NO_DIRS, NO_DIRS, // Dummy for EMPTY and PAWN
    KNIGHT_DIRS, BISHOP_DIRS, ROOK_DIRS, QUEEN_DIRS, KING_DIRS
};

void movegen_pieces(Board& board, Pieces& pieces, Moves& moves, 
		    bool captures_only, Color c, PieceType t) {
    Directions dirs = PIECES_DIRS[t];
    Directions::iterator it; 
    for (int i = 0; i < pieces.get_nb_pieces(c, t); ++i) {
	Square from = pieces.get_position(c, t, i);
	for (it = dirs.begin(); it != dirs.end(); it++) {
	    Square to = Square(from + *it);
	    while (!board.is_out(to)) {
		if (!board.is_empty(to)) {
		    if (board.get_piece(to).get_color() == c) break;
		    moves.add(Move(from, to, CAPTURE));
		    break;
		}
		else if (!captures_only) moves.add(Move(from, to));
		if (t == KNIGHT || t == KING) break; // Leapers
		to = Square(to + *it); // Sliders
	    }
	}
    }
}

Moves Game::movegen(bool captures_only) {
    Moves moves;
    Color c = current_node().get_turn_color();
    Directions::iterator it; 
    
    // Pawns moves
    for (int i = 0; i < pieces.get_nb_pieces(c, PAWN); ++i) {
	Direction d = (c == WHITE ? UP : DOWN);
	Square from = pieces.get_position(c, PAWN, i);
	
	// Pawn captures
	//Direction dirs[2] = { Direction(d + LEFT), Direction(d + RIGHT) };
	//for (int i = 0; i < 2; ++i) {
	Directions dirs = { Direction(d + LEFT), Direction(d + RIGHT) };
	for (it = dirs.begin(); it != dirs.end(); it++) {
	    //Square to = Square(from + dirs[i]);
	    Square to = Square(from + *it);
	    if (board.is_out(to)) continue;
	    if (!board.is_empty(to) && board.get_piece(to).get_color() != c) {
		if (board.is_pawn_end(c, to)) {
		    moves.add(Move(from, to, KNIGHT_PROMOTION_CAPTURE));
		    moves.add(Move(from, to, BISHOP_PROMOTION_CAPTURE));
		    moves.add(Move(from, to, ROOK_PROMOTION_CAPTURE));
		    moves.add(Move(from, to, QUEEN_PROMOTION_CAPTURE));
		}
		else {
		    moves.add(Move(from, to, CAPTURE));
		}
	    }
	    else if (to == current_node().get_en_passant()) {
		moves.add(Move(from, to, EN_PASSANT));
	    }
	}
	if (captures_only) continue;
	
	Square to = Square(from + d);
	assert(!board.is_out(to)); // Should never happend
	if (!board.is_empty(to)) continue;
	
	// Promotion
	if (board.is_pawn_end(c, to)) {
	    moves.add(Move(from, to, KNIGHT_PROMOTION));
	    moves.add(Move(from, to, BISHOP_PROMOTION));
	    moves.add(Move(from, to, ROOK_PROMOTION));
	    moves.add(Move(from, to, QUEEN_PROMOTION));
	    continue;
	}

	// Pawn push
	moves.add(Move(from, to));
	
	// Double pawn push
	if (board.is_pawn_begin(c, from)) {
	    to = Square(to + d);
	    if (!board.is_empty(to)) continue;
	    moves.add(Move(from, to, DOUBLE_PAWN_PUSH));
	}
    }

    // Standard moves
    movegen_pieces(board, pieces, moves, captures_only, c, KNIGHT);
    movegen_pieces(board, pieces, moves, captures_only, c, BISHOP);
    movegen_pieces(board, pieces, moves, captures_only, c, ROOK);
    movegen_pieces(board, pieces, moves, captures_only, c, QUEEN);
    movegen_pieces(board, pieces, moves, captures_only, c, KING);

    if (captures_only) return moves;
    
    // Castling
    if (current_node().can_castle(c, KING)) {
	Square from = Square(E1 + A8 * c);
	Square to = Square(G1 + A8 * c);
	if (!board.is_empty(Square(F1 + A8 * c)) &&
	    !board.is_empty(to)) { //&&
	    //!is_attacked_by(from) &&
	    //!is_attacked_by(Square((F1 + A8 * c))) &&
	    //!is_attacked_by(to)) {
	    moves.add(Move(from, to, KING_CASTLE));
	}
    }
    if (current_node().can_castle(c, QUEEN)) {
	Square from = Square(E1 + A8 * c);
	Square to = Square(C1 + A8 * c);
	if (!board.is_empty(Square(B1 + A8 * c)) &&
	    !board.is_empty(Square(D1 + A8 * c)) &&
	    !board.is_empty(to)) { //&&
	    //!is_attacked_by(from) &&
	    //!is_attacked_by(Square((D1 + A8 * c))) &&
	    //!is_attacked_by(to)) {
	    moves.add(Move(from, to, QUEEN_CASTLE));
	}
    }
    
    return moves;
}

void Game::make_move(Move m) {
    Square orig = m.get_orig();
    Square dest = m.get_dest();
    Piece capture;
    if (m.is_capture()) {
	Square s = dest;
	if (m.is_en_passant()) {
	    Square ep = current_node().get_en_passant();
	    Color c = current_node().get_turn_color();
	    s = (c == BLACK ? Square(ep + UP) : Square(ep + DOWN));
	}
	capture = board.get_piece(s);
	del_piece(capture);
    }
    // TODO: get color, and put new_node() here
    if (m.is_castle()) {
	Square rook_orig, rook_dest;
	Color c = current_node().get_turn_color();
	switch (m.get_castle_side()) {
	    case KING:
		rook_orig = Square(H1 + A8 * c); 
		rook_dest = Square(F1 + A8 * c); 
		break;
	    case QUEEN:
		rook_orig = Square(A1 + A8 * c); 
		rook_dest = Square(D1 + A8 * c); 
		break;
	    default:
		assert(false);
	}
	Piece rook = board.get_piece(rook_orig);
	board.set_piece(Piece(), rook_orig);
	board.set_piece(rook, rook_dest);
	pieces.set_position(rook, rook_dest);
	//current_node().set_castle_right(c, KING, false);
	//current_node().set_castle_right(c, QUEEN, false);
    }
    Piece p = board.get_piece(orig);
    board.set_piece(Piece(), orig);
    if (m.is_promotion()) {
	add_piece(p.get_color(), m.get_promotion_type(), dest);
	del_piece(p);
    }
    else {
	board.set_piece(p, dest);
	pieces.set_position(p, dest);
    }

    new_node();
    current_node().set_capture(capture);
    if (m.is_double_pawn_push()) {
	Square ep = Square(orig + (dest - orig) / 2);
	current_node().set_en_passant(ep);
    }
    else {
	current_node().set_en_passant(OUT);
    }
}

void Game::undo_move(Move m) {
    Square orig = m.get_orig();
    Square dest = m.get_dest();
    //cout << "o=" << orig << ", d=" << dest << endl;

    // Move back the piece to its origin
    Piece p = board.get_piece(dest);
    if (m.is_promotion()) {
	add_piece(p.get_color(), PAWN, orig);
	del_piece(p);
    }
    else {
	board.set_piece(p, orig);
	pieces.set_position(p, orig);
    }
    
    // Restore captured piece
    if (m.is_capture()) {
	Piece capture = current_node().get_capture();
	//cout << "Captured piece found: " << capture << endl;
	Square s = dest;
	if (m.is_en_passant()) {
	    Color c = current_node().get_turn_color();
	    s = (c == WHITE ? Square(dest + UP) : Square(dest + DOWN));
	    board.set_piece(Piece(), dest);
	}
	//cout << "Add " << capture << " to: " << s << endl;
	add_piece(capture.get_color(), capture.get_type(), s);
    }
    else {
	board.set_piece(Piece(), dest);
    }
    del_node();
    if (m.is_castle()) {
	Square rook_orig, rook_dest;
	Color c = current_node().get_turn_color();
	switch (m.get_castle_side()) {
	    case KING:
		rook_orig = Square(H1 + A8 * c); 
		rook_dest = Square(F1 + A8 * c); 
		break;
	    case QUEEN:
		rook_orig = Square(A1 + A8 * c); 
		rook_dest = Square(D1 + A8 * c); 
		break;
	    default:
		assert(false);
	}
	Piece rook = board.get_piece(rook_dest);
	board.set_piece(Piece(), rook_dest);
	board.set_piece(rook, rook_orig);
	pieces.set_position(rook, rook_orig);
    }
}
