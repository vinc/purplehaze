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

void Transpositions::clear() {
    for (int i = 0; i < SIZE; ++i) {
	tt[i] = Transposition();
    }
    hits = 0;
    misses = 0;
}

Transposition Transpositions::lookup(Hash h) {
    //assert(SIZE > 0);
    //assert(null_entry.is_empty());
    Transposition t = tt[h & (SIZE - 1)];
    
    // Verbose mode
    if (h && t.get_hash() == h) {
	++hits;
	return t;
    }
    if (h && !t.is_empty()) ++collisions;
    ++misses;
    return NULL_ENTRY;
    
    //return ((h && t.get_hash() == h) ? t : NULL_ENTRY);
}

int Transpositions::get_usage() const {
    int res = 0; 
    for (int i = 0; i < SIZE; ++i) if (!tt[i].is_empty()) ++res;
    return res;
}

