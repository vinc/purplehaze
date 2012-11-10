/* Copyright (C) 2007-2012 Vincent Ollivier
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

#include <cassert>
#include <iostream>

#include "game.h"

static bool king_castle_allowed(const Color c,
                                const Square from, const Square to,
                                const Board &board, const Pieces &pieces)
{
    assert(from == Board::flip(E1, c));
    assert(to == Board::flip(G1, c));
    const Square rook = Board::flip(H1, c);
    return
        board.is_empty(Board::flip(F1, c)) &&
        board.is_empty(to) &&
        board[rook].is(c, ROOK) &&
        !board.is_attacked_by(!c, from, pieces) &&
        !board.is_attacked_by(!c, to, pieces) &&
        !board.is_attacked_by(!c, Board::flip(F1, c), pieces);
}

static bool queen_castle_allowed(const Color c,
                                 const Square from, const Square to,
                                 const Board &board, const Pieces &pieces)
{
    assert(from == Board::flip(E1, c));
    assert(to == Board::flip(C1, c));
    const Square rook = Board::flip(A1, c);
    return
        board.is_empty(Board::flip(B1, c)) &&
        board.is_empty(Board::flip(D1, c)) &&
        board.is_empty(to) &&
        board[rook].is(c, ROOK) &&
        !board.is_attacked_by(!c, from, pieces) &&
        !board.is_attacked_by(!c, to, pieces) &&
        !board.is_attacked_by(!c, Board::flip(D1, c), pieces);
}

void Moves::generate_pieces(Color c, PieceType t, MoveType mt)
{
    for (int i = 0, n = pieces.count(c, t); i < n; ++i) {
        const Square from = pieces.position(c, t, i);
        for (const Direction &dir : PIECES_DIRS[t]) {
            Square to = static_cast<Square>(from + dir);
            while (!board.is_out(to)) {
                if (!board.is_empty(to)) {
                    if (board[to].is(c)) {
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
                to = static_cast<Square>(to + dir); // Sliders
            }
        }
    }
}

void Moves::generate(MoveType mt)
{
    const Color c = current_position.side();

    // Pawns moves
    for (int i = 0, n = pieces.count(c, PAWN); i < n; ++i) {
        const Square from = pieces.position(c, PAWN, i);

        // Pawn captures
        if (mt != QUIET_MOVE) {
            for (const Direction &dir : PAWN_CAPTURE_DIRS[c]) {
                const Square to = static_cast<Square>(from + dir);
                if (board.is_out(to)) {
                    continue;
                }
                if (!board.is_empty(to) && board[to].color() != c) {
                    if (board.is_pawn_end(c, to)) {
                        // Promotion capture
                        add(Move(from, to, KNIGHT_PROMOTION_CAPTURE));
                        add(Move(from, to, BISHOP_PROMOTION_CAPTURE));
                        add(Move(from, to, ROOK_PROMOTION_CAPTURE));
                        add(Move(from, to, QUEEN_PROMOTION_CAPTURE));
                    } else {
                        // Capture
                        add(Move(from, to, CAPTURE));
                    }
                } else if (to == current_position.en_passant()) {
                    // En passant
                    add(Move(from, to, EN_PASSANT));
                }
            }
        }

        if (mt == CAPTURE) {
            continue;
        }
        Square to = static_cast<Square>(from + PAWN_PUSH_DIRS[c]);
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
            to = static_cast<Square>(to + PAWN_PUSH_DIRS[c]);
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
    const Square from = Board::flip(E1, c);
    if (current_position.can_castle(c, KING)) {
        const Square to = Board::flip(G1, c);
        if (king_castle_allowed(c, from, to, board, pieces)) {
            add(Move(from, to, KING_CASTLE));
        }
    }
    if (current_position.can_castle(c, QUEEN)) {
        const Square to = Board::flip(C1, c);
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
    const Color c = current_position().side();
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
        if (t == KING || (t == ROOK && orig == Board::flip(H1, c))) {
            pos.set_castle_right(c, KING, false);
            zobrist.update_castle_right(pos.hash(), c, KING);
        }
    }
    if (pos.can_castle(c, QUEEN)) {
        if (t == KING || (t == ROOK && orig == Board::flip(A1, c))) {
            pos.set_castle_right(c, QUEEN, false);
            zobrist.update_castle_right(pos.hash(), c, QUEEN);
        }
    }

    // Capture
    if (m.is_capture()) {
        Square s = dest;
        if (m.is_en_passant()) {
            s = static_cast<Square>(ep + PAWN_PUSH_DIRS[!c]);
        }
        assert(!board.is_empty(s));

        Piece capture = board[s];
        if (capture.is(ROOK)) { // Update opponent's castling rights
            if (dest == Board::flip(H1, !c)) {
                pos.set_castle_right(!c, KING, false);
                zobrist.update_castle_right(pos.hash(), !c, KING);
            } else if (dest == Board::flip(A1, !c)) {
                pos.set_castle_right(!c, QUEEN, false);
                zobrist.update_castle_right(pos.hash(), !c, QUEEN);
            }
        }
        del_piece(capture);
        pos.set_capture(capture);
        assert(board.is_empty(s));
    }

    // Castling
    if (m.is_castle()) {
        Square rook_orig;
        Square rook_dest;
        switch (m.castle_side()) {
        case KING:
            rook_orig = Board::flip(H1, c);
            rook_dest = Board::flip(F1, c);
            break;
        case QUEEN:
            rook_orig = Board::flip(A1, c);
            rook_dest = Board::flip(D1, c);
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
    if (m.is_promotion()) {
        add_piece(p.color(), m.promotion_type(), dest);
        del_piece(p);
    } else {
        board[orig] = Piece();
        board[dest] = p;
        pieces.set_position(p, dest);
        zobrist.update_piece(pos.hash(), c, t, orig);
        zobrist.update_piece(pos.hash(), c, t, dest);
    }

    // Update en passant
    if (m.is_double_pawn_push()) {
        Square new_ep = static_cast<Square>((orig + dest) / 2);
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
            const Color c = current_position().side();
            s = static_cast<Square>(dest + PAWN_PUSH_DIRS[c]);
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
        const Color c = current_position().side();
        Square rook_orig;
        Square rook_dest;
        switch (m.castle_side()) {
        case KING:
            rook_orig = Board::flip(H1, c);
            rook_dest = Board::flip(F1, c);
            break;
        case QUEEN:
            rook_orig = Board::flip(A1, c);
            rook_dest = Board::flip(D1, c);
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

    const Piece p = board[from];
    const PieceType t = p.type();
    const Color c = p.color();

    // The piece cannot be one of the opponent
    if (c != current_position().side()) {
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
            s = static_cast<Square>(ep + PAWN_PUSH_DIRS[!c]);
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
        if (from != Board::flip(E1, c)) {
            return false;
        }
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
