/*  PurpleHaze 1.0
    Copyright (C) 2007-2009  Vincent Ollivier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <map>

#include "common.h"

#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

class Transposition
{
	private:
		Hash zobrist;
		Move best_move;
		unsigned char value;
		unsigned char depth;
		Bound bound;
	public:
		Transposition();
		Transposition(Hash h, int v, Bound b, int d, Move& bm);
		int get_value() const {return value;};
		Bound get_bound() const {return bound;};
		int get_depth() const {return depth;};
		Hash get_zobrist() const {return zobrist;};
		Move get_best_move() const {return best_move;};
};

/*

 Use a Transposition array of TT_SIZE.
 An index in the array should be :
 	i = zobrist_key % TT_SIZE
 And then the Transposition should also contain
 the full zobrist_key in order to find if it's
 the good move.
 If note we have to find a replacement strategie

 */


class Transpositions
{
	private:
		Transposition tt[TT_SIZE];
		//map<Hash,Transposition> tt;
		//map<Hash,Transposition>::iterator it;
		
	public:
		Transpositions();
		Transposition* lookup(Hash hash);
		void save(Hash h, int value, int alpha, int beta, int depth, Move& best_move);
		void save(Hash h, int value, Bound bound, int depth, Move& best_move);
};


#endif /* !TRANSPOSITIONS_H */
