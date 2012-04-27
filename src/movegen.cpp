/* Copyright (C) 2007-2011 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <iostream>

#include "game.h"

bool king_castle_allowed(const Color c, const Square from, const Square to,
                         const Board &board, const Pieces &pieces)
{
    assert(from == Square(E1 + A8 * c));
    assert(to == Square(G1 + A8 * c));
    const Square rook = Square(H1 + A8 * c);
    return
        board.is_empty(Square(F1 + A8 * c)) &&
        board.is_empty(to) &&
        board[rook].type() == ROOK &&
        board[rook].color() == c &&
        !board.is_attacked_by(!c, from, pieces) &&
        !board.is_attacked_by(!c, to, pieces) &&
        !board.is_attacked_by(!c, Square((F1 + A8 * c)), pieces);
}

bool queen_castle_allowed(const Color c, const Square from, const Square to,
                          const Board &board, const Pieces &pieces)
{
    assert(from == Square(E1 + A8 * c));
    assert(to == Square(C1 + A8 * c));
    const Square rook = Square(A1 + A8 * c);
    return
        board.is_empty(Square(B1 + A8 * c)) &&
        board.is_empty(Square(D1 + A8 * c)) &&
        board.is_empty(to) &&
        board[rook].type() == ROOK &&
        board[rook].color() == c &&
        !board.is_attacked_by(!c, from, pieces) &&
        !board.is_attacked_by(!c, to, pieces) &&
        !board.is_attacked_by(!c, Square((D1 + A8 * c)), pieces);
}

void Moves::generate_pieces(Color c, PieceType t, MoveType mt)
{
    const Direction * dirs = PIECES_DIRS[t];
    const int n = pieces.count(c, t);
    for (int i = 0; i < n; ++i) {
        const Square from = pieces.position(c, t, i);
        for (int d = 0; d < NB_DIRS[t]; ++d) {
            Square to = Square(from + dirs[d]);
            while (!board.is_out(to)) {
                if (!board.is_empty(to)) {
                    if (board[to].color() == c) {
                        break;
                    }
                    if (mt != QUIET_MOVE) {
                        add(Move(from, to, CAPTURE));
                    }
                    break;
                } else if (mt != CAPTURE) {
                    add(Move(from, to, QUIET_MOVE));
                }
                if (t == KNIGHT || t == KING) {
                    break; // Leapers
                }
                to = Square(to + dirs[d]); // Sliders
            }
        }
    }
}

void Moves::generate(MoveType mt)
{
    const Color c = current_position.turn_color();

    // Pawns moves
    const int n = pieces.count(c, PAWN);
    for (int i = 0; i < n; ++i) {
        Square from = pieces.position(c, PAWN, i);

        // Pawn captures
        for (int d = 0; d < 2; ++d) {
            if (mt == QUIET_MOVE) {
                break;
            }
            Square to = Square(from + PAWN_CAPTURE_DIRS[c][d]);
            if (board.is_out(to)) {
                continue;
            }
            if (!board.is_empty(to) && board[to].color() != c) {
                if (board.is_pawn_end(c, to)) { // Promotion capture
                    add(Move(from, to, KNIGHT_PROMOTION_CAPTURE));
                    add(Move(from, to, BISHOP_PROMOTION_CAPTURE));
                    add(Move(from, to, ROOK_PROMOTION_CAPTURE));
                    add(Move(from, to, QUEEN_PROMOTION_CAPTURE));
                } else { // Capture
                    add(Move(from, to, CAPTURE));
                }
            } else if (to == current_position.en_passant()) { // En passant
                add(Move(from, to, EN_PASSANT));
            }
        }

        if (mt == CAPTURE) {
            continue;
        }
        Square to = Square(from + PAWN_PUSH_DIRS[c]);
        assert(!board.is_out(to)); // Should never happend
        if (!board.is_empty(to)) {
            continue;
        }

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
            if (!board.is_empty(to)) {
                continue;
            }
            add(Move(from, to, DOUBLE_PAWN_PUSH));
        }
    }

    // Standard moves
    for (const PieceType& t : NOT_PAWN_TYPES) {
        generate_pieces(c, t, mt);
    }

    if (mt == CAPTURE) {
        return;
    }

    // Castling
    const Square from = Square(E1 + A8 * c);
    if (current_position.can_castle(c, KING)) {
        const Square to = Square(G1 + A8 * c);
        if (king_castle_allowed(c, from, to, board, pieces)) {
            add(Move(from, to, KING_CASTLE));
        }
    }
    if (current_position.can_castle(c, QUEEN)) {
        const Square to = Square(C1 + A8 * c);
        if (queen_castle_allowed(c, from, to, board, pieces)) {
            add(Move(from, to, QUEEN_CASTLE));
        }
    }
}

void Game::make_move(Move m)
{
    const Square orig = m.orig();
    const Square dest = m.dest();
    const Square ep = current_position().en_passant();
    const Color c = current_position().turn_color();
    const Piece p = board[orig];
    const PieceType t = p.type();
    assert(!board.is_out(orig));
    assert(!board.is_out(dest));

    ++nodes_count;
    new_position(); // current_position() is now refering to a new position
    Position& pos = current_position();

    // Update halfmove counter
    if (t == PAWN || m.is_capture()) {
        pos.reset_halfmove();
    } else {
        pos.inc_halfmove();
    }

    // Null Move
    if (m.is_null()) {
        pos.set_en_passant(OUT);
        return;
    }

    // Update castling rights
    if (pos.can_castle(c, KING)) {
        if (t == KING || (t == ROOK && orig == Square(H1 + A8 * c))) {
            pos.set_castle_right(c, KING, false);
            zobrist.update_castle_right(pos.hash(), c, KING);
        }
    }
    if (pos.can_castle(c, QUEEN)) {
        if (t == KING || (t == ROOK && orig == Square(A1 + A8 * c))) {
            pos.set_castle_right(c, QUEEN, false);
            zobrist.update_castle_right(pos.hash(), c, QUEEN);
        }
    }

    // Capture
    if (m.is_capture()) {
        Square s = dest;
        if (m.is_en_passant()) {
            s = (c == BLACK ? Square(ep + UP) : Square(ep + DOWN));
        }
        assert(!board.is_empty(s) || assert_msg(debug_move(m)));

        Piece capture = board[s];
        if (capture.type() == ROOK) { // Update opponent's castling rights
            if (dest == Square(H1 + A8 * !c)) {
                pos.set_castle_right(!c, KING, false);
                zobrist.update_castle_right(pos.hash(), !c, KING);
            } else if (dest == Square(A1 + A8 * !c)) {
                pos.set_castle_right(!c, QUEEN, false);
                zobrist.update_castle_right(pos.hash(), !c, QUEEN);
            }
        }
        del_piece(capture);
        pos.set_capture(capture);
        assert(board.is_empty(s) || assert_msg(debug_move(m)));
    }

    // Castling
    if (m.is_castle()) {
        Square rook_orig, rook_dest;
        switch (m.castle_side()) {
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
                rook_orig = OUT;
                rook_dest = OUT;
                break;
        }
        Piece rook = board[rook_orig];
        board[rook_orig] = Piece();
        board[rook_dest] = rook;
        pieces.set_position(rook, rook_dest);
        zobrist.update_piece(pos.hash(), c, ROOK, rook_orig);
        zobrist.update_piece(pos.hash(), c, ROOK, rook_dest);
        pos.set_has_castled(c); // For bonus/malus in eval
    }

    // Move the piece
    board[orig] = Piece(); // FIXME: duplicate in case of promotion?
    if (m.is_promotion()) {
        add_piece(p.color(), m.promotion_type(), dest);
        del_piece(p);
    } else {
        board[dest] = p;
        pieces.set_position(p, dest);
        zobrist.update_piece(pos.hash(), c, t, orig);
        zobrist.update_piece(pos.hash(), c, t, dest);
    }

    // Update en passant
    if (m.is_double_pawn_push()) {
        Square new_ep = Square(orig + (dest - orig) / 2);
        pos.set_en_passant(new_ep);
        zobrist.update_en_passant(pos.hash(), new_ep);
    } else {
        pos.set_en_passant(OUT);
    }
}

void Game::undo_move(Move m)
{
    Square orig = m.orig();
    Square dest = m.dest();

    // Move back the piece to its origin
    Piece p = board[dest];
    if (m.is_promotion()) {
        add_piece(p.color(), PAWN, orig);
        del_piece(p);
    } else if (!m.is_null()) {
        board[orig] = p;
        pieces.set_position(p, orig);
    }

    // Restore captured piece
    if (m.is_capture()) {
        Piece capture = current_position().capture();
        Square s = dest;
        if (m.is_en_passant()) {
            Color c = current_position().turn_color();
            s = (c == WHITE ? Square(dest + UP) : Square(dest + DOWN));
            board[dest] = Piece();
        }
        add_piece(capture.color(), capture.type(), s);
    } else if (!m.is_null()) {
        board[dest] = Piece();
    }
    del_position();
    if (m.is_null()) {
        return;
    }
    if (m.is_castle()) {
        Square rook_orig, rook_dest;
        Color c = current_position().turn_color();
        switch (m.castle_side()) {
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
                rook_orig = OUT;
                rook_dest = OUT;
                break;
        }
        Piece rook = board[rook_dest];
        board[rook_dest] = Piece();
        board[rook_orig] = rook;
        pieces.set_position(rook, rook_orig);
    }
}

/*
 * Check the pseudo legality of a move m
 */
bool Game::is_legal(Move m)
{
    // Null-move is obviously wrong
    if (m.is_null()) {
        return false;
    }

    Square from = m.orig();
    Square to = m.dest();

    // There must be a piece to move on the board
    if (board.is_empty(from)) {
        return false;
    }

    Piece p = board[from];
    PieceType t = p.type();
    Color c = p.color();

    // The piece cannot be one of the opponent
    if (c != current_position().turn_color()) {
        return false;
    }

    // It must be able to do the move
    if (!m.is_en_passant() && !m.is_castle()) {
        if (!board.can_go(p, from, to)) {
            return false;
        }
    }

    // Promotion
    if (t == PAWN && board.is_pawn_end(c, to) && !m.is_promotion()) {
        return false;
    }
    if (m.is_promotion()) {
        if (t != PAWN || !board.is_pawn_end(c, to)) {
            return false;
        }
    }

    // If it's a capture
    if (m.is_capture()) {
        Square s = to;

        // There are special conditions for en passant
        if (m.is_en_passant()) {
            if (t != PAWN) { // It must be a pawn
                return false;
            }
            Square ep = current_position().en_passant();
            if (to != ep) { // After a double push
                return false;
            }
            // from another pawn, the later being captured by the former
            s = (c == BLACK ? Square(ep + UP) : Square(ep + DOWN));
            if (board[s].type() != PAWN) {
                return false;
            }
        }

        // An opponent's piece must be captured
        if (board.is_empty(s)) {
            return false;
        }
        if (c == board[s].color()) {
            return false;
        }

    } else if (m.is_castle()) {
        switch (m.castle_side()) {
            case KING:
                return king_castle_allowed(c, from, to, board, pieces);
            case QUEEN:
                return queen_castle_allowed(c, from, to, board, pieces);
            default:
                return false;
        }
    } else if (m.is_double_pawn_push()) {
        if (t != PAWN) {
            return false;
        }
        if (!board.is_pawn_begin(c, from)) {
            return false; // Done by can_go()
        }
    } else if (!board.is_empty(to)) {
        return false;
    }

    // TODO: Add check
    return true;
}
