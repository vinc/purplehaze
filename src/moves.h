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

#ifndef MOVES_H
#define MOVES_H

#include "common.h"
#include "move.h"
#include "board.h"
#include "position.h"

using namespace std;


typedef char Score;

const int MAX_MOVES = 128; // TODO Find the real value
const Score BEST_SCORE = 127;
const Score KILLERS_SCORE = 0;

enum MovesState : unsigned char { 
    BEST, GOOD_CAPTURES, KILLERS, BAD_CAPTURES, QUIET_MOVES, UNDEF_MOVES
};

class Moves
{
    private:	
    	ExtendedMove moves[MAX_MOVES];

	Position& current_position;
	Board& board;
	Pieces& pieces;

	MovesState state;
	unsigned char i, n;
	unsigned char size[5]; // Number of differents moves'types

	bool use_lazy_generation;

    public:
	static Score mvv_lva_scores[KING + 1][KING + 1];
    	Moves(Board& b, Pieces& ps, Position& cn, bool lg = true) : 
	    current_position(cn), board(b), pieces(ps),
	    state(BEST),
	    i(0), n(0),
	    use_lazy_generation(lg)
	    { size = { 0 }; }

	void generate(MoveType mt = NULL_MOVE); // here NULL_MOVE => ALL_MOVE 
	void generate_pieces(Color c, PieceType t, MoveType mt);
	void add(Move m, MovesState mt = UNDEF_MOVES);
	ExtendedMove next();
	MovesState get_state() const { return state; };

	static void init_mvv_lva_scores();
	Score get_mvv_lva_score(Move m);	

	// Used for compatibility only
	/*
	int size() { 
	    return n;
	};
	Move at(int i) {
	    assert(i < n);
	    return moves[i];
	};
	*/

	// Used in divide
	void numeric_sort() { /*moves.sort(Move::numeric_comp);*/ };	
};

#endif /* !MOVES_H */
