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

#include "move.h"
#include "piece.h"

PieceType Move::promotion_type() const
{
    switch (type()) {
    case KNIGHT_PROMOTION:
    case KNIGHT_PROMOTION_CAPTURE:
        return KNIGHT;
    case BISHOP_PROMOTION:
    case BISHOP_PROMOTION_CAPTURE:
        return BISHOP;
    case ROOK_PROMOTION:
    case ROOK_PROMOTION_CAPTURE:
        return ROOK;
    case QUEEN_PROMOTION:
    case QUEEN_PROMOTION_CAPTURE:
        return QUEEN;
    default:
        return EMPTY;
    }
}

PieceType Move::castle_side() const
{
    switch (type()) {
    case KING_CASTLE:
        return KING;
    case QUEEN_CASTLE:
        return QUEEN;
    default:
        return EMPTY;
    }
}

std::ostream& operator<<(std::ostream& out, const Move move)
{
    return (out << move.to_string());
}

std::string Move::to_string() const
{
    if (is_null()) {
        return "#";
    }
    std::string res = "";
    res += static_cast<char>('a' + orig_file());
    res += static_cast<char>('1' + orig_rank());
    res += static_cast<char>('a' + dest_file());
    res += static_cast<char>('1' + dest_rank());
    if (is_promotion()) {
        res += Piece(BLACK, promotion_type()).to_string(); // Lower case
    }
    return res;
}
