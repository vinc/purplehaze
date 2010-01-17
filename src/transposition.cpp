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
	size = (TT_SIZE / sizeof(Transposition)) - 1;
	this->clear_stats();
}

void Transpositions::clear() {
	for (int i = 0; i < size; ++i) {
		Move none_move(0, OUT, OUT);
		Transposition empty(0, 0, UNDEF_BOUND, 0, none_move);
		tt[i] = empty;
	}
	clear_stats();
}

void Transpositions::clear_stats() {
	lookups_counter = 0;
	hits_counter = 0;
	collisions_counter = 0;
	replacements_counter = 0;
}

Transposition* Transpositions::lookup(Hash hash) {
	int index = hash & size;
	Transposition* ptr_trans = &tt[hash & size];
	//++lookups_counter;
	//cout << "TT: " << sizeof(*ptr_trans) << endl; 
	// Return 56 if move is a ref, 24 for a pointer
	// But the use of the pointer slow done very much
	/*
	cout << endl;
	cout << "Lookup @" << index << " (#" << lookups_counter << ")" << endl;
	*/
	
	if (ptr_trans /* ? added for test in 1.0.7 */ && ptr_trans->get_zobrist() == hash) {
		//++hits_counter;
		/*
		cout << "Got " << ptr_trans->get_zobrist() << endl;
		cout << "Cmp " << hash << endl;
		cout << endl;
		*/
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
	/*
	if (value > +INF - 100) value = +INF;
	else if (value < -INF + 100) value = -INF;
	*/
	
	Transposition trans(hash, value, bound, depth, best_move);
	
	// Always replace strategie
	tt[hash & size] = trans;
}

void Transpositions::save(Hash hash, int value, Bound bound, int depth, Move& best_move) {
	//if (value > +INF - 100) value = +INF;
	//else if (value < -INF + 100) value = -INF;

	Transposition trans(hash, value, bound, depth, best_move);

	//++replacements_counter;
	//int h = (int) hash;

	/*
	int index = hash & size;
	cout << "Replace: " << tt[index].get_zobrist() << " (#" << replacements_counter << ")" << endl;
	cout << "With   : " << hash << endl;
	cout << "@      : " << index << endl;
	cout << "Of     : " << size << endl;
	cout << endl;
	*/
	
	// Always replace strategie
	//Hash old_hash = tt[hash & size].get_zobrist();
	//if (old_hash != 0 && old_hash != hash) {
		//cout << "Collision!" << endl;
		//cout << "Hash: " << hash << endl;
		//cout << "TT.Hash: " << tt[hash % TT_SIZE].get_zobrist() << endl;
		
	//	++collisions_counter;
	//}
	//if (tt[hash % TT_SIZE].get_depth() < trans.get_depth())
		tt[hash & size] = trans;
}

float Transpositions::get_usage() const {
	float used_entry = 0;
	for (int i = 0; i < size; ++i) {
		if (tt[i].get_best_move().get_ptr_piece()) {
			++used_entry;
		}
	}
	return used_entry * 100 / float(size);
}