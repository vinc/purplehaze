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

#ifndef PIECES_H
#define PIECES_H

#include <assert.h>

#include "common.h"
#include "piece.h"

class Pieces
{
    private:
	Square positions[2][7][9];
	unsigned char nb_pieces[2][7];
	unsigned char sum[2];
    
    public:
	Pieces();
	Square get_position(Piece p) const {
	    return positions[p.get_color()][p.get_type()][p.get_index()];
	};
	Square get_position(Color c, PieceType t, int i) const {
	    assert(0 <= i && i < 9);
	    return positions[c][t][i];
	};
	void set_position(Piece p, Square s) {
	    positions[p.get_color()][p.get_type()][p.get_index()] = s;
	};
	void set_position(Color c, PieceType t, int i, Square s) { 
	    assert(0 <= i && i < 9);
	    positions[c][t][i] = s;
	};
	unsigned char get_nb_pieces(Color c, PieceType t) const {
	    return nb_pieces[c][t];
	};
	unsigned char get_nb_pieces(Color c) const {
	    return sum[c];
	};

	// FIXME? No warning if nb < 0 or nb > 9
	void inc_nb_pieces(Color c, PieceType t) {
	    ++nb_pieces[c][t];
	    ++sum[c];
	};
	void dec_nb_pieces(Color c, PieceType t) {
	    --nb_pieces[c][t];
	    --sum[c];
	};
};

#endif /* !PIECESL_H */
