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

#include <cassert>

#include "game.h"

/*
 * Return true if the square s is attacked by one of the c color's pieces.
 */
bool Board::is_attacked_by(const Color c, const Square s,
                           const Pieces& pieces) const
{
    for (const PieceType& t : NOT_PAWN_TYPES) {
        for (int i = 0, n = pieces.count(c, t); i < n; ++i) {
            const Square from = pieces.position(c, t, i);
            if (!can_attack(t, from, s)) {
                continue;
            }
            if (t == KNIGHT || t == KING) {
                return true;
            }
            const Direction d = direction_to(from, s);
            Square to = static_cast<Square>(from + d);
            while (to != s && is_empty(to)) {
                to = static_cast<Square>(to + d);
            }
            if (to == s) {
                return true;
            }
        }
    }

    // Specific code for pawns
    for (const Direction &d : PAWN_CAPTURE_DIRS[!c]) {
        const Square from = static_cast<Square>(s + d);
        if (board[from].is(c, PAWN)) {
            return true;
        }
    }

    return false;
}

/*
 * Return 'true' if a piece 'p' can do a capture or a quiet move from
 * square 'from' to square 'to' on the board. This function does not
 * say if the move is legal.
 */
bool Board::can_go(const Piece p, const Square from, const Square to) const
{
    const Color c = p.color();

    // A piece cannot capture another piece of the same color
    if (!is_empty(to) && board[to].is(c)) {
        return false;
    }

    Direction d;
    Square s;
    const PieceType t = p.type();
    switch (t) {
    case PAWN:
        d = PAWN_PUSH_DIRS[c];
        s = static_cast<Square>(from + d);
        if (!is_empty(to)) { // Capture
            if (to == static_cast<Square>(s + LEFT)) {
                return true;
            }
            if (to == static_cast<Square>(s + RIGHT)) {
                return true;
            }
        } else { // Pawn push (and double push)
            if (to == s) {
                return true;
            }
            if (to == static_cast<Square>(s + d)) {
                if (is_empty(s) && is_pawn_begin(c, from)) {
                    return true;
                }
            }
        }
        break;
    default:
        if (!can_attack(t, from, to)) {
            return false;
        }
        if (t == KNIGHT || t == KING) {
            return true;
        }
        d = direction_to(from, to);
        s = static_cast<Square>(from + d);
        while (s != to && is_empty(s)) { // Search for a blocker
            s = static_cast<Square>(s + d);
            assert(!is_out(s));
        }
        if (s == to) {
            return true;
        }
        break;
    }
    return false;
}
