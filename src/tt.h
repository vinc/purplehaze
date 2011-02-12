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

#include <string>

#include "common.h"
#include "move.h"
#include "zobrist.h"

enum Bound : unsigned char { EXACT, LOWER, UPPER, UNDEF_BOUND };

class Transposition
{
    private:
	Hash hash;		// 64 bits needed
	short value;		// 16 bits needed
	Move best_move;		// 16 bits needed
	unsigned char depth;	// 10 bits needed (depth < 512)
	Bound bound;		// 3 bits needed

    public:
	Transposition(Hash h, int v, Bound b, int d, Move bm)
	    : hash(h), value(v), best_move(bm), depth(d), 
	      bound(b) {}
	Transposition() 
	    : hash(Hash()), value(0), best_move(Move()), depth(0), 
	      bound(UNDEF_BOUND) {}
	
	Hash get_hash() const { return hash; };
	int get_value() const { return value; };
	Bound get_bound() const { return bound; };
	int get_depth() const { return depth; };
	Move get_best_move() const { return best_move; };
	bool is_empty() const;
	string to_string() const;
};

class Transpositions
{
    private:
	int hits;
	int collisions;
	int misses;
	const int SIZE;
	Transposition* tt;
	const Transposition NULL_ENTRY;

    public:
	Transpositions(int n = TT_SIZE) : 
	    hits(0),
	    collisions(0),
	    misses(0),
	    SIZE(n / sizeof(Transposition)),
	    tt(new Transposition[SIZE])
	    {}
	~Transpositions() {
	    delete [] tt;
	    tt = NULL;
	};
	Transposition lookup(Hash hash);
	void save(Hash h, int v, Bound b, int d, Move bm) {
	    tt[h & (SIZE - 1)] = Transposition(h, v, b, d, bm);
	};

	void clear();

	// Used only for unit testing
	int size() const { return SIZE; };
	Transposition& at(int i) const { return tt[i]; };

	// Used to print stats
	int get_usage() const;
	int get_nb_lookups() const { return hits + misses; };
	int get_nb_hits() const { return hits; };
	int get_nb_collisions() const { return collisions; };
	int get_nb_misses() const { return misses; };
};

#endif /* !TT_H */
