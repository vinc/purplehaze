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

#include "hashtable.h"

template <class T>
T HashTable<T>::lookup(Hash h, bool& is_empty) {
    Entry entry = entries[h & (SIZE - 1)];
    if (entry.hash == h) {
        is_empty = false;
        ++hits;
        return entry.value;
    }
    else if (entry.hash != 0) ++collisions;
    is_empty = true;
    ++misses;
    return T();
}

template <class T>
void HashTable<T>::clear() {
    for (int i = 0; i < SIZE; ++i) {
        entries[i].hash = 0;
        entries[i].value = T();
    }
    hits = 0;
    misses = 0;
}

template <class T>
int HashTable<T>::get_usage() const {
    int res = 0; 
    for (int i = 0; i < SIZE; ++i) if (entries[i].hash) ++res;
    return res;
}

template class HashTable<int>; // To avoid linker errors
