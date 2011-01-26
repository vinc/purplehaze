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
    for (int i = 0; i < SIZE; ++i) tt[i] = Transposition();
}

Transposition& Transpositions::lookup(Hash h) {
    assert(tt != NULL);
    assert((h & (SIZE - 1)) < SIZE);
    Transposition t = tt[h & (SIZE - 1)];
    //cout << ">" << t.get_hash() << " / " << h << endl;
    //if (t.get_bound() < 0 || 2 < t.get_bound()) return 0;
    //return (h && t.get_hash() == h ? t : 0);
    return (h && t.get_hash() == h ? t : null_entry);
}
void Transpositions::save(Hash h, int v, Bound b, int d, Move bm) {
    tt[h & (SIZE - 1)] = Transposition(h, v, b, d, bm);
    //cout << "<" << /*t.get_hash() << " / " <<*/ h;
    //cout << " b=" << b << ", bm=" << bm << endl;
}

void Transpositions::save(Hash h, int v, int a, int b, int d, Move bm) {
    assert((h & (SIZE - 1)) < SIZE);
    Bound bound = (v <= a ? UPPER : (v >= b ? LOWER : EXACT));
    tt[h & (SIZE - 1)] = Transposition(h, v, bound, d, bm); // Always Replace
    cout << "<" << /*t.get_hash() << " / " <<*/ h << endl;
}
