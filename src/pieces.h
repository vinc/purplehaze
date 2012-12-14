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

#ifndef PIECES_H
#define PIECES_H

#include <cassert>

#include "common.h"
#include "piece.h"

class Pieces
{
private:
    Square positions[2][7][9];
    unsigned char nb_pieces[2][7];
    unsigned char total[2];

public:
    Pieces() : nb_pieces(), total() {
        std::fill(&positions[0][0][0],
                  &positions[0][0][0] + sizeof(positions),
                  OUT);
    }

    Square position(Piece p) const {
        assert(0 <= p.index() && p.index() < 9);
        return positions[p.color()][p.type()][p.index()];
    }
    Square position(Color c, PieceType t, int i) const {
        assert(0 <= i && i < 9);
        return positions[c][t][i];
    }
    void set_position(Piece p, Square s) {
        assert(0 <= p.index() && p.index() < 9);
        positions[p.color()][p.type()][p.index()] = s;
    }
    void set_position(Color c, PieceType t, int i, Square s) {
        assert(0 <= i && i < 9);
        positions[c][t][i] = s;
    }
    unsigned char count(Color c, PieceType t) const {
        return nb_pieces[c][t];
    }
    unsigned char count(Color c) const {
        return total[c];
    }

    // FIXME? No warning if nb < 0 or nb > 9
    void inc_nb_pieces(Color c, PieceType t) {
        ++nb_pieces[c][t];
        ++total[c];
    }
    void dec_nb_pieces(Color c, PieceType t) {
        --nb_pieces[c][t];
        --total[c];
    }
};

#endif /* !PIECESL_H */
