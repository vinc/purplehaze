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

#include "game.h"

/*
 * Return true if the square s is attacked by one of the c color's pieces.
 */
bool Board::is_attacked_by(Color c, Square s, const Pieces& pieces) const
{
    for (const PieceType& t : NOT_PAWN_TYPES) {
        int n = pieces.count(c, t);
        for (int i = 0; i < n; ++i) {
            Square from = pieces.get_position(c, t, i);
            if (!can_attack(t, from, s)) continue;
            if (t == KNIGHT || t == KING) return true;
            Direction d = get_direction_to(from, s);
            Square to = Square(from + d);
            while (to != s && is_empty(to)) {
                to = Square(to + d);
            }
            if (to == s) return true;
        }
    }

    // Specific code for pawns
    for (int i = 0; i < 2; ++i) {
        Square from = Square(s + PAWN_CAPTURE_DIRS[!c][i]);
        if (board[from].type() == PAWN && board[from].color() == c) {
            return true;
        }
    }

    return false;
}

/*
 * Return true if a piece p can do a capture or a quiet move from square from
 * to square to on the board. This method does not say if the move is a legal
 * one.
 */
bool Board::can_go(Piece p, Square from, Square to) const
{
    PieceType t = p.type();
    Color c = p.color();
    Direction d = get_direction_to(from, to);

    // A piece cannot capture another piece of the same color
    if (!is_empty(to) && board[to].color() == c) return false;

    Direction push_dir;
    Square s;
    switch (t) {
        case PAWN:
            push_dir = (c == WHITE ? UP : DOWN);
            if (!is_empty(to)) { // Capture
                if (to == Square(from + push_dir + LEFT)) return true;
                if (to == Square(from + push_dir + RIGHT)) return true;
            } else { // Pawn push (and double push)
                if (to == Square(from + push_dir)) return true;
                if (to == Square(from + 2 * push_dir) &&
                    is_empty(Square(from + push_dir)) &&
                    is_pawn_begin(c, from)) return true;
            }
            break;
        default:
            if (!can_attack(t, from, to)) return false;
            if (t == KNIGHT || t == KING) return true;
            s = Square(from + d);
            while (s != to && is_empty(s)) { // Search for a blocker
                s = Square(s + d);
                assert(!is_out(s));
            }
            if (s == to) return true;
            break;
    }
    return false;
}
