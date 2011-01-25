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

#include "tt.h"

using namespace std;

Transpositions::Transpositions() {
    cout << "Transposition Table's size: " << (TT_SIZE / (1024*1024));
    cout << "Mb (" << SIZE << " entries)" << endl;
}

void Transpositions::clear() {
    for (int i = 0; i < SIZE; ++i) tt[i] = Transposition();
}

Transposition* Transpositions::lookup(Hash h) {
    assert((h & (SIZE - 1)) < SIZE);
    Transposition t = tt[h & (SIZE - 1)];
    return (t.get_hash() == h ? &t : 0);
}

void Transpositions::save(Hash h, int v, int a, int b, int d, Move bm) {
    assert((h & (SIZE - 1)) < SIZE);
    Bound bound = (v <= a ? UPPER : (v >= b ? LOWER : EXACT));
    tt[h & (SIZE - 1)] = Transposition(h, v, bound, d, bm); // Always Replace
}
