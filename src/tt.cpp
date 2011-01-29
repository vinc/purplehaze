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

#include <iostream>
#include <assert.h>
#include <sstream>
#include <string>

#include "tt.h"

using namespace std;

bool Transposition::is_empty() const {
    if (get_hash() != 0 || get_value() != 0 || get_depth() != 0 ||
	get_bound() != UNDEF_BOUND || !get_best_move().is_null()) {	
	return false;
    }
    return true;
}

string Transposition::to_string() const {
    ostringstream stream;
    stream << "<" << hex << get_hash();
    stream << ", " << dec << get_value() << ", " << get_depth();
    stream << ", " << get_bound() << ", " << get_best_move() << ">";
    return stream.str();
}
/*
Transpositions::Transpositions(int n) {
    if (n == 0) n = TT_SIZE;
    assert((n & (n - 1)) == 0); // n must be a power of two
    SIZE = n / sizeof(Transposition);
    tt = new Transposition[tt_size];
    cout << "Transposition Table's size: " << (n / (1024*1024));
    cout << "Mb (" << SIZE << " entries)" << endl;
    clear();
}
*/

void Transpositions::clear() {
    //cout << "Cleaning the " << SIZE << " entries in TT" << endl;
    for (int i = 0; i < SIZE; ++i) {
	tt[i] = Transposition();
	//assert(tt[i].is_empty());
    }
}

Transposition Transpositions::lookup(Hash h) {
    //assert(SIZE > 0);
    //assert(null_entry.is_empty());
    Transposition t = tt[h & (SIZE - 1)];
    //cout << "look " << t.to_string() << endl;
    return ((h && t.get_hash() == h) ? t : null_entry);
}
void Transpositions::save(Hash h, int v, Bound b, int d, Move bm) {
    //cout << "cast <" << hex << h << ", " << dec << v << ", " << d << ", ";
    //cout << b << ", " << bm << ">" << endl;
    Transposition t(h, v, b, d, bm);
    /*
    assert(h == t.get_hash());
    assert(v == t.get_value());
    assert(b == t.get_bound());
    assert(bm == t.get_best_move());
    */
    //cout << "save " << t.to_string() << endl;
    tt[h & (SIZE - 1)] = t;
}

/*
void Transpositions::save(Hash h, int v, int a, int b, int d, Move bm) {
    Bound bound = (v <= a ? UPPER : (v >= b ? LOWER : EXACT));
    tt[h & (SIZE - 1)] = Transposition(h, v, bound, d, bm); // Always Replace
}
*/
