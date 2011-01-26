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

#ifndef TT_H
#define TT_H

#include "common.h"
#include "move.h"
#include "zobrist.h"

//#define TT_SIZE	2048 //1024*1024*128 // Transpositions table of 128Mb

static const int TT_SIZE = 1024*1024*128;

enum Bound : unsigned char { EXACT = 0, LOWER, UPPER, UNDEF_BOUND };

class Transposition
{
    private:
	Hash hash;
	unsigned char value;
	Bound bound;
	unsigned char depth;
	Move best_move;

    public:
	//Transposition()
	//    : hash(0), value(0), bound(UNDEF_BOUND), depth(d), best_move(Move()) {}
	Transposition(Hash h, int v, Bound b, int d, Move bm)
	    : hash(h), value(v), bound(b), depth(d), best_move(bm) {}
	Transposition() 
	    : hash(Hash()), value(0), bound(UNDEF_BOUND), depth(0), 
	      best_move(Move()) {}
	
	Hash get_hash() const { return hash; };
	int get_value() const { return value; };
	Bound get_bound() const { return bound; };
	int get_depth() const { return depth; };
	Move get_best_move() const { return best_move; };
};

class Transpositions
{
    private:
	//static const int SIZE = (TT_SIZE / sizeof(Transposition));
	//Transposition tt[SIZE];
	const int SIZE;
	Transposition* tt; // = new Transposition[SIZE];
	Transposition null_entry;

    public:
	Transpositions(int n = TT_SIZE) : SIZE(n / sizeof(Transposition)) {
	    assert((n & (n - 1)) == 0);
	    tt = new Transposition[SIZE];
	};
	~Transpositions() {
	    delete [] tt;
	    tt = NULL;
	};
	Transposition& lookup(Hash hash);
	void save(Hash h, int v, int a, int b, int d, Move bm);
	void save(Hash h, int v, Bound b, int d, Move bm); /* {
	    tt[h & (SIZE - 1)] = Transposition(h, v, b, d, bm);
	}*/
	void clear();

	// Used only for unit testing
	int size() const { return SIZE; };
	Transposition& at(int i) const { return tt[i]; };
};

#endif /* !TT_H */
