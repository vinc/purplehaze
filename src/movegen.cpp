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

#include "game.h"

using namespace std;

void Moves::generate_pieces(Color c, PieceType t, MoveType mt) {
    const Direction * dirs = PIECES_DIRS[t];
    for (int j = 0; j < pieces.get_nb_pieces(c, t); ++j) {
	Square from = pieces.get_position(c, t, j);
	for (int d = 0; d < NB_DIRS[t]; ++d) {
	    Square to = Square(from + dirs[d]);
	    while (!board.is_out(to)) {
		if (!board.is_empty(to)) {
		    if (board.get_piece(to).get_color() == c) break;
		    if (mt != QUIET_MOVE) add(Move(from, to, CAPTURE));
		    break;
		}
		else if (mt != CAPTURE) add(Move(from, to, QUIET_MOVE));
		if (t == KNIGHT || t == KING) break; // Leapers
		to = Square(to + dirs[d]); // Sliders
	    }
	}
    }
}

void Moves::generate(MoveType mt) {
    Color c = current_node.get_turn_color();
    
    // Pawns moves
    for (int j = 0; j < pieces.get_nb_pieces(c, PAWN); ++j) {
	Square from = pieces.get_position(c, PAWN, j);

	// Pawn captures
	for (int d = 0; d < 2; ++d) {
	    if (mt == QUIET_MOVE) break;
	    Square to = Square(from + PAWN_CAPTURE_DIRS[c][d]);
	    if (board.is_out(to)) continue;
	    if (!board.is_empty(to) && board.get_piece(to).get_color() != c) {
		if (board.is_pawn_end(c, to)) { // Promotion capture
		    add(Move(from, to, KNIGHT_PROMOTION_CAPTURE));
		    add(Move(from, to, BISHOP_PROMOTION_CAPTURE));
		    add(Move(from, to, ROOK_PROMOTION_CAPTURE));
		    add(Move(from, to, QUEEN_PROMOTION_CAPTURE));
		}
		else { // Capture
		    add(Move(from, to, CAPTURE));
		}
	    }
	    else if (to == current_node.get_en_passant()) { // En passant
		add(Move(from, to, EN_PASSANT));
	    }
	}
	
	if (mt == CAPTURE) continue;
	Square to = Square(from + PAWN_PUSH_DIRS[c]);
	assert(!board.is_out(to)); // Should never happend
	if (!board.is_empty(to)) continue;
	
	// Promotion
	if (board.is_pawn_end(c, to)) {
	    add(Move(from, to, KNIGHT_PROMOTION));
	    add(Move(from, to, BISHOP_PROMOTION));
	    add(Move(from, to, ROOK_PROMOTION));
	    add(Move(from, to, QUEEN_PROMOTION));
	    continue;
	}

	// Pawn push
	add(Move(from, to, QUIET_MOVE));
	
	// Double pawn push
	if (board.is_pawn_begin(c, from)) {
	    to = Square(to + PAWN_PUSH_DIRS[c]);
	    if (!board.is_empty(to)) continue;
	    add(Move(from, to, DOUBLE_PAWN_PUSH));
	}
    }

    // Standard moves
    for (PieceType t = KNIGHT; t <= KING; t = PieceType(t + 1)) {
	generate_pieces(c, t, mt);
    }

    if (mt == CAPTURE) return;
    
    // Castling
    if (current_node.can_castle(c, KING)) {
	Square from = Square(E1 + A8 * c);
	Square to = Square(G1 + A8 * c);
	Square rook = Square(H1 + A8 * c);
	if (board.is_empty(Square(F1 + A8 * c)) &&
	    board.is_empty(to) &&
	    board.get_piece(rook).get_type() == ROOK &&
	    board.get_piece(rook).get_color() == c &&
	    !board.is_attacked_by(Color(!c), from, pieces) &&
	    !board.is_attacked_by(Color(!c), to, pieces) &&
	    !board.is_attacked_by(Color(!c), Square((F1 + A8 * c)), pieces)
	    ) {
	    add(Move(from, to, KING_CASTLE));
	}
    }
    if (current_node.can_castle(c, QUEEN)) {
	Square from = Square(E1 + A8 * c);
	Square to = Square(C1 + A8 * c);
	Square rook = Square(A1 + A8 * c);
	if (board.is_empty(Square(B1 + A8 * c)) &&
	    board.is_empty(Square(D1 + A8 * c)) &&
	    board.is_empty(to) &&
	    board.get_piece(rook).get_type() == ROOK &&
	    board.get_piece(rook).get_color() == c &&
	    !board.is_attacked_by(Color(!c), from, pieces) &&
	    !board.is_attacked_by(Color(!c), to, pieces) &&
	    !board.is_attacked_by(Color(!c), Square((D1 + A8 * c)), pieces)
	    ) {
	    add(Move(from, to, QUEEN_CASTLE));
	}
    }
}

void Game::make_move(Move m) {
    Square orig = m.get_orig();
    Square dest = m.get_dest();
    Square ep = current_node().get_en_passant();
    Color c = current_node().get_turn_color();
    Piece p = board.get_piece(orig);
    PieceType t = p.get_type();
    Piece capture;
    assert(!board.is_out(orig));
    assert(!board.is_out(dest));

    ++nodes_count;
    new_node(); // From now on, current_node() is refering to the new node

    // Update halfmove counter
    if (t == PAWN || m.is_capture()) current_node().reset_halfmove();
    else current_node().inc_halfmove();
    
    // Null Move
    if (m.is_null()) {
	current_node().set_en_passant(OUT);
	return;
    }

    // Update castling rights
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
    
    // Capture
    if (m.is_capture()) {
	Square s = dest;
	if (m.is_en_passant()) {
	    s = (c == BLACK ? Square(ep + UP) : Square(ep + DOWN));
	}
	assert(!board.is_empty(s) || assert_msg(debug_move(m)));

	capture = board.get_piece(s);
	if (capture.get_type() == ROOK) { // Update opponent's castling rights
	    Color oc = Color(!c); // Opponent's color
	    if (dest == Square(H1 + A8 * c)) {
		current_node().set_castle_right(oc, KING, false);
		zobrist.update_castle_right(current_node().hash(), oc, KING);
	    }
	    else if (dest == Square(A1 + A8 * c)) {
		current_node().set_castle_right(oc, QUEEN, false);
		zobrist.update_castle_right(current_node().hash(), oc, QUEEN);
	    }
	}
	del_piece(capture);
	assert(board.is_empty(s) || assert_msg(debug_move(m)));
    }

    // Castling
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
	current_node().set_has_castle(c); // For bonus/malus in eval
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
	Square new_ep = Square(orig + (dest - orig) / 2);
	current_node().set_en_passant(new_ep);
	zobrist.update_en_passant(current_node().hash(), new_ep);
    }
    else {
	current_node().set_en_passant(OUT);
    }
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
    else if (!m.is_null()) {
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
    else if (!m.is_null()) {
	board.set_piece(Piece(), dest);
    }
    del_node();
    if (m.is_null()) return;
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

/*
 * Check the pseudo legality of a move m
 */
bool Game::is_legal(Move m) {
    // Null-move is obviously wrong
    if (m.is_null()) return false;
    
    Square from = m.get_orig();
    Square to = m.get_dest();
    
    // There must be a piece to move on the board
    if (board.is_empty(from)) return false;

    Piece p = board.get_piece(from);
    PieceType t = p.get_type();
    Color c = p.get_color();

    // The piece cannot be one of the opponent
    if (c != current_node().get_turn_color()) return false;
    
    // It must be able to do the move
    if (!m.is_en_passant() && 
	!m.is_castle() && 
	!board.can_go(p, from, to)) return false;
    
    // Promotion
    if (t == PAWN && board.is_pawn_end(c, to) && !m.is_promotion()) {
	return false;
    }
    if (m.is_promotion()) {
	if (t != PAWN || !board.is_pawn_end(c, to)) return false;
    }

    // If it's a capture
    if (m.is_capture()) {
	Square s = to;

	// There are special conditions for en passant
	if (m.is_en_passant()) {
	    // It must be a pawn
	    if (t != PAWN) return false;

	    // After a double push
	    Square ep = current_node().get_en_passant();
	    if (to != ep) return false;
	    
	    // from another pawn, the later being captured by the former
	    s = (c == BLACK ? Square(ep + UP) : Square(ep + DOWN));   
	    if (board.get_piece(s).get_type() != PAWN) return false;
	}

	// An opponent's piece must be captured
	if (board.is_empty(s)) return false;
	if (c == board.get_piece(s).get_color()) return false;

    }
    else if (m.is_castle()) {
	Square rook = Square(H1 + A8 * c);
	Color oc = Color(!c); // Opponent's color
	switch (m.get_castle_side()){
	    case KING:
		rook = Square(H1 + A8 * c);
		if (!(board.is_empty(Square(F1 + A8 * c)) &&
		    board.is_empty(to) &&
		    board.get_piece(rook).get_type() == ROOK &&
		    board.get_piece(rook).get_color() == c &&
		    !board.is_attacked_by(oc, from, pieces) &&
		    !board.is_attacked_by(oc, Square((F1 + A8 * c)), pieces) &&
		    !board.is_attacked_by(oc, to, pieces))
		    ) {
		    return false;
		}
		break;
	    case QUEEN:
		rook = Square(A1 + A8 * c);
		if (!(board.is_empty(Square(B1 + A8 * c)) &&
		    board.is_empty(Square(D1 + A8 * c)) &&
		    board.is_empty(to) &&
		    board.get_piece(rook).get_type() == ROOK &&
		    board.get_piece(rook).get_color() == c &&
		    !board.is_attacked_by(oc, from, pieces) &&
		    !board.is_attacked_by(oc, Square((D1 + A8 * c)), pieces) &&
		    !board.is_attacked_by(oc, to, pieces))
		    ) {
		    return false;
		}
		break;
	    default: return false;
	}
	return true;
    }
    else if (m.is_double_pawn_push()) {
	if (t != PAWN) return false;
	if (!board.is_pawn_begin(c, from)) return false; // Done by can_go()
    }
    else if (!board.is_empty(to)) return false;
    return true;
}
