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

void movegen_pieces(Board& board, Pieces& pieces, Moves& moves, 
		    bool captures_only, Color c, PieceType t) {
    const Direction * dirs = PIECES_DIRS[t];
    for (int i = 0; i < pieces.get_nb_pieces(c, t); ++i) {
	Square from = pieces.get_position(c, t, i);
	for (int i = 0; i < NB_DIRS[t]; ++i) {
	    Square to = Square(from + dirs[i]);
	    while (!board.is_out(to)) {
		if (!board.is_empty(to)) {
		    if (board.get_piece(to).get_color() == c) break;
		    moves.add(Move(from, to, CAPTURE));
		    break;
		}
		else if (!captures_only) moves.add(Move(from, to, QUIET_MOVE));
		if (t == KNIGHT || t == KING) break; // Leapers
		to = Square(to + dirs[i]); // Sliders
	    }
	}
    }
}

Moves Game::movegen(bool captures_only) {
    Moves moves;
    Color c = current_node().get_turn_color();
    
    // Pawns moves
    for (int i = 0; i < pieces.get_nb_pieces(c, PAWN); ++i) {
	Square from = pieces.get_position(c, PAWN, i);

	// Pawn captures
	for (int i = 0; i < 2; ++i) {
	    Square to = Square(from + PAWN_CAPTURE_DIRS[c][i]);
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
	Square to = Square(from + PAWN_PUSH_DIRS[c]);
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
	moves.add(Move(from, to, QUIET_MOVE));
	
	// Double pawn push
	if (board.is_pawn_begin(c, from)) {
	    to = Square(to + PAWN_PUSH_DIRS[c]);
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
	Square rook = Square(H1 + A8 * c);
	if (board.is_empty(Square(F1 + A8 * c)) &&
	    board.is_empty(to) &&
	    board.get_piece(rook).get_type() == ROOK &&
	    board.get_piece(rook).get_color() == c &&
	    !is_attacked_by(Color(!c), from) &&
	    !is_attacked_by(Color(!c), Square((F1 + A8 * c))) &&
	    !is_attacked_by(Color(!c), to)
	    ) {
	    moves.add(Move(from, to, KING_CASTLE));
	}
    }
    if (current_node().can_castle(c, QUEEN)) {
	Square from = Square(E1 + A8 * c);
	Square to = Square(C1 + A8 * c);
	Square rook = Square(A1 + A8 * c);
	if (board.is_empty(Square(B1 + A8 * c)) &&
	    board.is_empty(Square(D1 + A8 * c)) &&
	    board.is_empty(to) &&
	    board.get_piece(rook).get_type() == ROOK &&
	    board.get_piece(rook).get_color() == c &&
	    !is_attacked_by(Color(!c), from) &&
	    !is_attacked_by(Color(!c), Square((D1 + A8 * c))) &&
	    !is_attacked_by(Color(!c), to)
	    ) {
	    moves.add(Move(from, to, QUEEN_CASTLE));
	}
    }
    
    return moves;
}

void Game::make_move(Move m) {
    Square orig = m.get_orig();
    Square dest = m.get_dest();
    Square ep = current_node().get_en_passant();
    Color c = current_node().get_turn_color();
    Piece p = board.get_piece(orig);
    Piece capture;
    assert(!board.is_out(orig));
    assert(!board.is_out(dest));

    nodes_count++;
    new_node(); // From now on, current_node() is refering to the new node
    
    // Update castling rights
    PieceType t = p.get_type();
    if ((current_node().can_castle(c, KING)) && 
	(t == KING || (t == ROOK && orig == Square(H1 + A8 * c)))) {
	current_node().set_castle_right(c, KING, false);
	zobrist.update_castle_right(current_node().hash(), c, KING);
    }
    if ((current_node().can_castle(c, QUEEN)) && 
	(t == KING || (t == ROOK && orig == Square(A1 + A8 * c)))) {
	current_node().set_castle_right(c, QUEEN, false);
	zobrist.update_castle_right(current_node().hash(), c, QUEEN);
    }
    
    if (m.is_capture()) {
	Square s = dest;
	if (m.is_en_passant()) {
	    s = (c == BLACK ? Square(ep + UP) : Square(ep + DOWN));
	}
	assert(!board.is_empty(s));
	capture = board.get_piece(s);
	if (capture.get_type() == ROOK) { // Update opponent's castling rights
	    if (dest == Square(H1 + A8 * c)) {
		current_node().set_castle_right(Color(!c), KING, false);
		zobrist.update_castle_right(current_node().hash(), 
					 Color(!c), KING);
	    }
	    else if (dest == Square(A1 + A8 * c)) {
		current_node().set_castle_right(Color(!c), QUEEN, false);
		zobrist.update_castle_right(current_node().hash(), 
					 Color(!c), QUEEN);
	    }
	}
	del_piece(capture);
	assert(board.is_empty(s));
    }

    if (m.is_castle()) {
	Square rook_orig, rook_dest;
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
	zobrist.update_piece(current_node().hash(), c, ROOK, rook_orig);
	zobrist.update_piece(current_node().hash(), c, ROOK, rook_dest);
    }

    // Move the piece
    board.set_piece(Piece(), orig); // FIXME: duplicate in case of promotion?
    if (m.is_promotion()) {
	add_piece(p.get_color(), m.get_promotion_type(), dest);
	del_piece(p);
    }
    else {
	board.set_piece(p, dest);
	pieces.set_position(p, dest);
	zobrist.update_piece(current_node().hash(), c, t, orig);
	zobrist.update_piece(current_node().hash(), c, t, dest);
    }
    
    // Update en passant
    current_node().set_capture(capture);
    if (m.is_double_pawn_push()) {
	Square ep = Square(orig + (dest - orig) / 2);
	current_node().set_en_passant(ep);
	zobrist.update_en_passant(current_node().hash(), ep);
    }
    else {
	current_node().set_en_passant(OUT);
    }
    
    // Update halfmove counter
    if (m.is_capture() || t == PAWN) current_node().reset_halfmove();
    else current_node().inc_halfmove();
}

void Game::undo_move(Move m) {
    Square orig = m.get_orig();
    Square dest = m.get_dest();
    
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
	Square s = dest;
	if (m.is_en_passant()) {
	    Color c = current_node().get_turn_color();
	    s = (c == WHITE ? Square(dest + UP) : Square(dest + DOWN));
	    board.set_piece(Piece(), dest);
	}
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
	    default: assert(false);
	}
	Piece rook = board.get_piece(rook_dest);
	board.set_piece(Piece(), rook_dest);
	board.set_piece(rook, rook_orig);
	pieces.set_position(rook, rook_orig);
    }
}

bool Game::is_legal_move(Move m) {
    // TODO
    return true;
}
