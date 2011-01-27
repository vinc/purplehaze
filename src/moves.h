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

#include <algorithm>
#include <list>

#include "common.h"
#include "move.h"

using namespace std;

class Moves
{
    private:
	//list<ExtendedMove> moves;
	static const int MAX_MOVES = 200; // TODO Find the real value
	ExtendedMove moves[MAX_MOVES];
	int n;
    public:
	Moves() : n(0) {}
	void add(Move m) { 
	    //moves.push_back(m);
	    moves[n] = m;
	    ++n;
	};
	int size() const { return n; };
	Move at(int i) const { return moves[i]; };
	//list<ExtendedMove>::iterator it;
	
	//list<ExtendedMove>::iterator begin() { return moves.begin(); };
	//list<ExtendedMove>::iterator end() { return moves.end(); };

	void numeric_sort() { /*moves.sort(Move::numeric_comp);*/ };
	void selection_sort();
	void sort(Move bm);
};

#endif /* !MOVES_H */
