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

#ifndef BOARD_H
#define BOARD_H

#include <bitset>

#include "common.h"
#include "pieces.h"

class Board
{
    friend ostream& operator<<(ostream& out, const Board board);
    private:
	Piece board[BOARD_SIZE];
	bitset<7> attack_array[240];
	Direction dir_array[240];

    public:
	Board();
	Piece get_piece(Square s) const {
	    return board[s];
	};
	void set_piece(Piece p, Square s) { 
	    board[s] = p;
	};
	bool is_empty(Square s) const { 
	    return board[s].get_type() == EMPTY; 
	};
	bool is_out(Square s) const { 
	    return s & 0x88; 
	};
	bool is_dark(Square s) const {
	    return (s & 7) % 2 == (s >> 4) % 2;
	};
	Square get_square(Square s, Direction d) const { 
	    return Square(s + d);
	};
	File get_file(Square s) const { 
	    return File(s & 7); 
	};
	Rank get_rank(Square s) const { 
	    return Rank(s >> 4); 
	};
	bool is_pawn_begin(Color c, Square s) const {
	    return (get_rank(s) - 5 * c) == 1;
	};
	bool is_pawn_end(Color c, Square s) const {
	    return (get_rank(s) + 7 * c) == 7;
	};

	// Theoretical answer by array lookup
	bool can_attack(PieceType t, Square from, Square to) const {
	    return bool(attack_array[0x77 + from - to][t]);
	}
	Direction get_direction_to(Square from, Square to) const {
	    return dir_array[0x77 + from - to];
	}

	// Practical answer
	bool is_attacked_by(Color c, Square s, const Pieces& pieces) const;
	bool can_go(Piece p, Square from, Square to) const; 

};

#endif /* !BOARD_H */
