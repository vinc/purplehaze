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

#include <iostream>

#include "piece.h"

std::ostream& operator<<(std::ostream& out, const Piece piece)
{
    return (out << piece.to_string());
}

std::string Piece::to_string() const
{
    char t;
    switch (type()) {
        case PAWN:   t = 'P'; break;
        case KNIGHT: t = 'N'; break;
        case BISHOP: t = 'B'; break;
        case ROOK:   t = 'R'; break;
        case QUEEN:  t = 'Q'; break;
        case KING:   t = 'K'; break;
        default:     t = ' '; break;
    }
    if (color() == BLACK) {
        t = static_cast<char>(t + 'a' - 'A'); // Lower case for black pieces
    }
    return std::string(1, t);
}
