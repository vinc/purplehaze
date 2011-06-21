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

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "common.h"
#include "zobrist.h"

template <class T>
class HashTable
{
    protected:
        int hits;
        int collisions;
        int misses;
        const int SIZE;
        struct Entry {
            Hash hash;
            T value;
        };
        Entry* entries;

    public:
        HashTable(int used_space = MT_SIZE) : 
            hits(0),
            collisions(0),
            misses(0),
            SIZE(used_space / sizeof(Entry)),
            entries(new Entry[SIZE])
            {}
        ~HashTable() {
            delete [] entries;
            entries = NULL;
        };
        T lookup(Hash h, bool& is_empty); /* {
            Entry entry = entries[h & (SIZE - 1)];
            if (entry.hash == h) return entry.value;
            else return T();
        };*/
        void save(Hash h, T v) {
            entries[h & (SIZE - 1)].hash = h;
            entries[h & (SIZE - 1)].value = v;
        };
        void clear();

        // Used to print stats
        int size() const { return SIZE; };

        int get_usage() const;
        int get_nb_lookups() const { return hits + misses; };
        int get_nb_hits() const { return hits; };
        int get_nb_collisions() const { return collisions; };
        int get_nb_misses() const { return misses; };
};

#endif /* !HASH_TABLE_H */
