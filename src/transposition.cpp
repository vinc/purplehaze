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
#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "transposition.h"

using namespace std;

Transposition::Transposition() {
	zobrist = 0;
	value = 0;
	depth = 0;
	bound = UNDEF_BOUND;
}

Transposition::Transposition(Hash h, int v, Bound b, int d, Move& bm) : 
	zobrist(h), best_move(bm), value(v), depth(d), bound(b) {
	//TODO
}
/*
int Transposition::get_value() const {
	return value;
}

Bound Transposition::get_bound() const {
	return bound;
}

int Transposition::get_depth() const {
	return depth;
}

Hash Transposition::get_zobrist() const {
	return zobrist;
}

Move Transposition::get_best_move() const {
	return best_move;
}
*/

Transpositions::Transpositions() {
	//TODO
	//for (int i = 0; i < TT_SIZE; ++i) tt[i] = 0;
}

Transposition* Transpositions::lookup(Hash hash) {
	Transposition* ptr_trans = &tt[hash % TT_SIZE];

	//cout << "TT: " << sizeof(*ptr_trans) << endl; 
	// Return 56 if move is a ref, 24 for a pointer
	// But the use of the pointer slow done very much

	if (ptr_trans /* ? added for test in 1.0.7 */ && ptr_trans->get_zobrist() == hash) {
		return ptr_trans;
	}
	else {
		return 0;
	}
}

void Transpositions::save(Hash hash, int value, int alpha, int beta, int depth, Move& best_move) {
	Bound bound;
	if (value <= alpha) {
		bound = UPPER;
	}
	else if (value >= beta) {
		bound = LOWER;
	}
	else {
		bound = EXACT;
	}
	
	if (value > +INF - 100) value = +INF;
	else if (value < -INF + 100) value = -INF;

	Transposition trans(hash, value, bound, depth, best_move);
	
	// Always replace strategie
	tt[hash % TT_SIZE] = trans;
}

void Transpositions::save(Hash hash, int value, Bound bound, int depth, Move& best_move) {
	if (value > +INF - 100) value = +INF;
	else if (value < -INF + 100) value = -INF;

	Transposition trans(hash, value, bound, depth, best_move);
	
	// Always replace strategie
	tt[hash % TT_SIZE] = trans;
}